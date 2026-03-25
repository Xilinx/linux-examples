#!/usr/bin/env python3
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

"""
NoC Configuration and Device Tree Generator
Processes .rnpi file to generate Device Tree files:
1. Auto-detect device part from .rnpi header
2. Run cdoutil to generate versal.txt
3. Parse configuration and generate Device Tree files
"""

import os
import sys
import subprocess
import argparse
import traceback
from collections import OrderedDict


# Constants
OUTPUT_FILE = "noc_config.txt"
DT_REGION_NAME = ""  # Empty for default (no suffix)

# Register offsets and sizes
PCSR_LOCK_OFFSET = 0xC
NOC_REG_BLOCK_SIZE = 0x1000

# Hardware topology limits
MAX_NMU_INSTANCES = 60
MAX_DDRMC_INSTANCES = 4
MAX_NSU_PORTS = 4

# NSU port destination ID values indicating disabled/unassigned ports
NSU_DEST_ID_DISABLED = "0xfc0"
NSU_DEST_ID_UNASSIGNED = "0x0"

# Maximum lines to search in .rnpi file header
RNPI_HEADER_MAX_LINES = 20


class DeviceTreeGenerator:
    """Helper class for generating Device Tree files."""

    @staticmethod
    def addr_hi(addr):
        """Extract upper 32 bits of address."""
        return (addr >> 32) & 0xffffffff

    @staticmethod
    def addr_lo(addr):
        """Extract lower 32 bits of address."""
        return addr & 0xffffffff

    @staticmethod
    def gen_nmu_node(nmu_id, addr, indent):
        """Generate DT node for NMU component."""
        lo = DeviceTreeGenerator.addr_lo(addr)
        hi = DeviceTreeGenerator.addr_hi(addr)
        lines = [
            f'{indent}nmu@{lo:x} {{',
            f'{indent}\treg = <0x{hi:x} 0x{lo:x} 0x0 0x{NOC_REG_BLOCK_SIZE:x}>;',
            f'{indent}\tamd,nmu-id = <{nmu_id}>;',
            f'{indent}}};',
        ]
        return '\n'.join(lines)

    @staticmethod
    def gen_ddrmc_node(ddrmc_id, addr, ports, indent):
        """Generate DT node for DDRMC NSU component."""
        lo = DeviceTreeGenerator.addr_lo(addr)
        hi = DeviceTreeGenerator.addr_hi(addr)
        ports_str = ' '.join(str(p) for p in sorted(ports))
        lines = [
            f'{indent}ddrmc-nsu@{lo:x} {{',
            f'{indent}\treg = <0x{hi:x} 0x{lo:x} 0x0 0x{NOC_REG_BLOCK_SIZE:x}>;',
            f'{indent}\tamd,ddrmc-id = <{ddrmc_id}>;',
            f'{indent}\tamd,nsu-ports = <{ports_str}>;',
            f'{indent}}};',
        ]
        return '\n'.join(lines)

    @staticmethod
    def gen_dt_content(region_name, nmu_list, nsu_list):
        """Generate Device Tree include file content (.dtsi)."""
        indent = '\t\t'
        parts = []

        # Generate NMU nodes
        for nmu_id, nmu_addr_str in nmu_list:
            addr = int(nmu_addr_str, 16)
            parts.append(DeviceTreeGenerator.gen_nmu_node(nmu_id, addr, indent))

        # Group DDRMC_NSU entries by (base_addr, ddrmc_id)
        ddrmc_groups = OrderedDict()
        for ddrmc_base_addr_str, ddrmc_id, port_num in nsu_list:
            addr = int(ddrmc_base_addr_str, 16)
            key = (addr, ddrmc_id)
            if key not in ddrmc_groups:
                ddrmc_groups[key] = []
            ddrmc_groups[key].append(port_num)

        # Generate DDRMC nodes
        for (addr, ddrmc_id), ports in ddrmc_groups.items():
            parts.append(DeviceTreeGenerator.gen_ddrmc_node(ddrmc_id, addr, ports, indent))

        children = '\n\n'.join(parts)

        # Generate node name (hyphens in DT, will be converted to underscores by driver)
        node_name = f'amd-noc-pmu-{region_name}' if region_name else 'amd-noc-pmu'
        region_comment = f" - region: {region_name}" if region_name else ""

        return f"""\
// SPDX-License-Identifier: GPL-2.0
// Auto-generated from NoC configuration{region_comment}

/ {{
\t{node_name} {{
\t\tcompatible = "amd,noc-pmu";
\t\t#address-cells = <2>;
\t\t#size-cells = <2>;
\t\tranges;

{children}
\t}};
}};
"""

    @staticmethod
    def gen_dt_overlay_content(region_name, nmu_list, nsu_list):
        """Generate Device Tree overlay file content (.dtso)."""
        indent = '\t\t'
        parts = []

        # Generate NMU nodes
        for nmu_id, nmu_addr_str in nmu_list:
            addr = int(nmu_addr_str, 16)
            parts.append(DeviceTreeGenerator.gen_nmu_node(nmu_id, addr, indent))

        # Group DDRMC_NSU entries by (base_addr, ddrmc_id)
        ddrmc_groups = OrderedDict()
        for ddrmc_base_addr_str, ddrmc_id, port_num in nsu_list:
            addr = int(ddrmc_base_addr_str, 16)
            key = (addr, ddrmc_id)
            if key not in ddrmc_groups:
                ddrmc_groups[key] = []
            ddrmc_groups[key].append(port_num)

        # Generate DDRMC nodes
        for (addr, ddrmc_id), ports in ddrmc_groups.items():
            parts.append(DeviceTreeGenerator.gen_ddrmc_node(ddrmc_id, addr, ports, indent))

        children = '\n\n'.join(parts)

        # Generate node name (hyphens in DT, will be converted to underscores by driver)
        node_name = f'amd-noc-pmu-{region_name}' if region_name else 'amd-noc-pmu'
        region_comment = f" - region: {region_name}" if region_name else ""

        return f"""\
// SPDX-License-Identifier: GPL-2.0
// Auto-generated from NoC configuration{region_comment}

/dts-v1/;
/plugin/;

&{{/}} {{
\t#address-cells = <2>;
\t#size-cells = <2>;

\t{node_name} {{
\t\tcompatible = "amd,noc-pmu";
\t\t#address-cells = <2>;
\t\t#size-cells = <2>;
\t\tranges;

{children}
\t}};
}};
"""


class NoCConfigGenerator:
    """Main class for generating NoC configuration files."""

    def __init__(self, input_file, output_file, region_name=DT_REGION_NAME):
        self.input_file = input_file
        self.output_file = output_file
        self.region_name = region_name
        # Generate filenames
        if region_name:
            self.dtsi_output_file = f'amd-noc-pmu-{region_name}.dtsi'
            self.dtso_output_file = f'amd-noc-pmu-{region_name}.dtso'
        else:
            self.dtsi_output_file = 'amd-noc-pmu.dtsi'
            self.dtso_output_file = 'amd-noc-pmu.dtso'
        self.lines = []

    def read_input_file(self):
        """Read the input configuration file."""
        directory_path = os.getcwd()
        text_file_path = os.path.join(directory_path, self.input_file)

        if not os.path.exists(text_file_path):
            raise FileNotFoundError(f"Input file not found: {text_file_path}")

        with open(text_file_path, 'r') as f:
            self.lines = f.readlines()

    def process_ddr_nsu_data(self):
        """
        Process DDR NSU instances and generate configuration data.

        Returns:
            list: List of tuples (ddrmc_base_addr, ddrmc_id, port_num)
        """
        nsu_list = []

        for ddrmc_idx in range(MAX_DDRMC_INSTANCES):
            # Check if DDRMC instance is enabled
            ddrmc_noc_state = f"DDRMC_NOC_{ddrmc_idx}.pcsr_lock.STATE[0]=0x0"
            is_enabled = False
            ddrmc_base_addr = ""

            for line_idx, line in enumerate(self.lines):
                if ddrmc_noc_state in line:
                    is_enabled = True
                    if line_idx + 1 >= len(self.lines):
                        raise ValueError(
                            f"DDRMC_NOC_{ddrmc_idx}: missing address line after state entry"
                        )
                    fields = self.lines[line_idx + 1].split()
                    if len(fields) < 2:
                        raise ValueError(
                            f"DDRMC_NOC_{ddrmc_idx}: malformed address line: "
                            f"{self.lines[line_idx + 1].strip()!r}"
                        )
                    addr_val = int(fields[1], 16)
                    ddrmc_base_addr = hex(addr_val - PCSR_LOCK_OFFSET)
                    break

            if not is_enabled:
                continue

            # Process each NSU port for this DDRMC
            for port_idx in range(MAX_NSU_PORTS):
                nsu_entry = self.process_ddrmc_port(ddrmc_idx, port_idx, ddrmc_base_addr)
                if nsu_entry:
                    nsu_list.append(nsu_entry)

        return nsu_list

    def process_ddrmc_port(self, ddrmc_idx, port_idx, ddrmc_base_addr):
        """
        Process a single DDRMC NSU port.

        Args:
            ddrmc_idx: DDRMC instance index
            port_idx: Port index
            ddrmc_base_addr: DDRMC_NOC base address

        Returns:
            tuple or None: (ddrmc_base_addr, ddrmc_idx, port_idx) or None if not valid
        """
        nsu_port_pattern = f"DDRMC_NOC_{ddrmc_idx}.reg_nsu{port_idx}_port.src_id[11:0]"

        nsu_dest_id = None

        for line in self.lines:
            if nsu_port_pattern in line:
                nsu_dest_id = line.split("=")[1].strip()
                break

        # Skip disabled or non-DDR NSU ports
        if not nsu_dest_id or nsu_dest_id in (NSU_DEST_ID_DISABLED, NSU_DEST_ID_UNASSIGNED):
            return None

        return (ddrmc_base_addr, ddrmc_idx, port_idx)

    def process_nmu_data(self):
        """
        Process NMU instances and generate configuration data.

        Returns:
            list: List of tuples (nmu_id, nmu_address)
        """
        nmu_list = []

        for nmu_idx in range(MAX_NMU_INSTANCES):
            pattern = f"NOC_NMU_{nmu_idx}.REG_PCSR_LOCK.STATE[0]=0x1"

            for line_idx, line in enumerate(self.lines):
                if pattern in line:
                    if line_idx + 1 >= len(self.lines):
                        raise ValueError(
                            f"NOC_NMU_{nmu_idx}: missing address line after state entry"
                        )
                    fields = self.lines[line_idx + 1].split()
                    if len(fields) < 2:
                        raise ValueError(
                            f"NOC_NMU_{nmu_idx}: malformed address line: "
                            f"{self.lines[line_idx + 1].strip()!r}"
                        )
                    addr_val = int(fields[1], 16)
                    nmu_address = hex(addr_val - PCSR_LOCK_OFFSET)

                    nmu_list.append((nmu_idx, nmu_address))
                    break

        return nmu_list

    def generate_files(self):
        """Main method to generate configuration and Device Tree files."""
        self.read_input_file()

        # Process DDR NSU data
        nsu_list = self.process_ddr_nsu_data()

        # Process NMU data
        nmu_list = self.process_nmu_data()

        # Write configuration file
        with open(self.output_file, 'w') as f:
            f.write("# NoC Configuration File\n")
            f.write("# Auto-generated from versal.txt\n")
            f.write("# Format: NMU <id> <address_in_hex>\n")
            f.write("#         DDRMC_NSU <ddrmc_base_addr_in_hex> <ddrmc_id> <port>\n")
            f.write("\n")

            # Write NMU components
            if nmu_list:
                f.write("# NMU components\n")
                for nmu_id, nmu_addr in nmu_list:
                    f.write(f"NMU {nmu_id} {nmu_addr}\n")
                f.write("\n")

            # Write DDRMC NSU components
            if nsu_list:
                f.write("# DDRMC NSU components\n")
                for ddrmc_base_addr, ddrmc_id, port_num in nsu_list:
                    f.write(f"DDRMC_NSU {ddrmc_base_addr} {ddrmc_id} {port_num}\n")

        print(f"Generated {self.output_file} successfully")
        print(f"  - {len(nmu_list)} NMU components")
        print(f"  - {len(nsu_list)} DDRMC NSU components")

        # Generate Device Tree file
        self.generate_dt_file(nmu_list, nsu_list)

    def generate_dt_file(self, nmu_list, nsu_list):
        """Generate Device Tree files (.dtsi and .dtso) from NoC configuration."""
        # Generate DTSI content (for static inclusion)
        dtsi_content = DeviceTreeGenerator.gen_dt_content(
            self.region_name, nmu_list, nsu_list
        )

        # Generate DTSO overlay content (for dynamic loading)
        dtso_content = DeviceTreeGenerator.gen_dt_overlay_content(
            self.region_name, nmu_list, nsu_list
        )

        # Write DTSI file
        with open(self.dtsi_output_file, 'w') as f:
            f.write(dtsi_content)

        # Write DTSO file
        with open(self.dtso_output_file, 'w') as f:
            f.write(dtso_content)

        print(f"Generated {self.dtsi_output_file}")
        print(f"Generated {self.dtso_output_file}")
        if self.region_name:
            print(f"  - Device Tree files for region '{self.region_name}'")
        else:
            print(f"  - Device Tree include file (.dtsi) for static inclusion")
            print(f"  - Device Tree overlay file (.dtso) ready to compile to .dtbo")


# ============================================================================
# RNPI Processing Functions
# ============================================================================

def parse_rnpi_header(rnpi_path):
    """
    Parse .rnpi file header to extract device part number.

    Args:
        rnpi_path: Path to .rnpi file

    Returns:
        str: Device part number (e.g., 'xcvc1902')

    Raises:
        ValueError: If part number cannot be extracted
    """
    try:
        with open(rnpi_path, 'r') as f:
            for i, line in enumerate(f):
                if i > RNPI_HEADER_MAX_LINES:
                    break

                # Look for line like: "Part:           xcvc1902-vsva2197-2MP-e-S"
                if line.startswith('Part:'):
                    # Extract full part number
                    full_part = line.split(':')[1].strip()
                    # Extract base device (first component before '-')
                    device = full_part.split('-')[0]
                    print(f"Detected device: {device} (from {full_part})")
                    return device

        raise ValueError("Part number not found in .rnpi header")

    except Exception as e:
        raise ValueError(f"Failed to parse .rnpi file: {e}")


def run_cdoutil(rnpi_path, device, output_file='versal.txt'):
    """
    Run cdoutil to convert .rnpi to annotated text file.

    Args:
        rnpi_path: Path to .rnpi file
        device: Device part number (e.g., 'xcvc1902')
        output_file: Output text file name

    Returns:
        bool: True if successful
    """
    cmd = [
        'cdoutil',
        '-dev', device,
        '-annotate',
        '-output-file', output_file,
        rnpi_path
    ]

    print(f"\nRunning cdoutil...")
    print(f"  Command: {' '.join(cmd)}")

    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(f"Successfully generated {output_file}")
        if result.stdout:
            print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error running cdoutil: {e}", file=sys.stderr)
        if e.stdout:
            print(e.stdout)
        if e.stderr:
            print(e.stderr, file=sys.stderr)
        return False
    except FileNotFoundError:
        print("Error: cdoutil not found. Please ensure it's in your PATH.", file=sys.stderr)
        print("Refer to UG1283 for cdoutil installation.", file=sys.stderr)
        return False


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description='Generate NoC Device Tree files from .rnpi file',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Usage:

The script generates Device Tree files for AMD Versal NoC performance monitoring
from a .rnpi file.

The script will:
  1. Extract the device part number from .rnpi header
  2. Run cdoutil to generate versal.txt
  3. Parse versal.txt and generate Device Tree files (.dtsi and .dtso)

Multiple NoC regions:
  Use --region to create separate PMU instances for different NoC regions:

  python3 generate_noc_dt.py design.rnpi --region fabric

  This generates amd-noc-pmu-fabric.dtsi / amd-noc-pmu-fabric.dtso, etc.

Examples:
  # Basic usage (device auto-detected)
  python3 generate_noc_dt.py design.rnpi

  # With region name
  python3 generate_noc_dt.py design.rnpi --region fabric

  # Manual device specification
  python3 generate_noc_dt.py design.rnpi --device xcvc1902
"""
    )

    parser.add_argument('rnpi_file',
                       help='Path to .rnpi file')
    parser.add_argument('--device', '-d',
                       help='Device part number (auto-detected if not specified)')
    parser.add_argument('--region', '-r',
                       help='Region name for multiple NoC instances')
    parser.add_argument('--output', '-o', default='versal.txt',
                       help='Output text file name (default: versal.txt)')

    args = parser.parse_args()

    region_name = args.region or DT_REGION_NAME

    try:
        # Check if .rnpi file exists
        if not os.path.exists(args.rnpi_file):
            print(f"Error: .rnpi file not found: {args.rnpi_file}", file=sys.stderr)
            return 1

        print("=" * 70)
        print("NoC Device Tree Generation")
        print("=" * 70)
        print(f"Input: {args.rnpi_file}")
        if region_name:
            print(f"Region: {region_name}")
        print()

        # Step 1: Extract device part number (if not provided)
        device = args.device
        if not device:
            print("Step 1: Parsing .rnpi header...")
            try:
                device = parse_rnpi_header(args.rnpi_file)
            except ValueError as e:
                print(f"Error: {e}", file=sys.stderr)
                print("\nPlease specify device manually with --device option", file=sys.stderr)
                return 1
        else:
            print(f"Step 1: Using specified device: {device}")

        # Step 2: Run cdoutil
        print("\nStep 2: Converting .rnpi to annotated text file...")
        if not run_cdoutil(args.rnpi_file, device, args.output):
            return 1

        # Step 3: Generate Device Tree files
        print("\nStep 3: Generating Device Tree files...")
        generator = NoCConfigGenerator(args.output, OUTPUT_FILE, region_name)
        generator.generate_files()

        # Success message
        print("\n" + "=" * 70)
        print("SUCCESS: Device Tree files generated")
        print("=" * 70)

        if region_name:
            dtsi_file = f"amd-noc-pmu-{region_name}.dtsi"
            dtso_file = f"amd-noc-pmu-{region_name}.dtso"
        else:
            dtsi_file = "amd-noc-pmu.dtsi"
            dtso_file = "amd-noc-pmu.dtso"

        print("\nGenerated files:")
        print(f"  - {args.output} (annotated NoC register dump)")
        print(f"  - {dtsi_file} (for static Device Tree inclusion)")
        print(f"  - {dtso_file} (for runtime overlay)")
        print("\nNext steps:")
        print(f"  1. Compile overlay: dtc -@ -I dts -O dtb -o {dtso_file[:-2]}bo {dtso_file}")
        print(f"  2. Apply overlay or include {dtsi_file} in your system DT")
        print("\nRefer to README for detailed usage instructions.")

        return 0

    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
