==============================
AMD NoC Performance Monitoring
==============================

This project provides tools to configure AMD Versal Network-on-Chip (NoC)
performance monitoring through Device Tree. It enables performance monitoring
of NMU (Network Master Unit) and DDRMC-NSU (DDR Memory Controller Network
Slave Unit) components through the Linux perf subsystem.

The project consists of:

- Python script (generate_noc_dt.py) - Generates NoC Device Tree files from
  .rnpi file (auto-detects device, runs cdoutil)

Prerequisites
=============
Build Requirements:

- Python 3.6+ for Device Tree generation
- Device Tree Compiler (dtc) version 1.4.7+ with overlay support
- CDO utility (cdoutil) for converting .rnpi files to annotated text files
- Perf tool installed on target system

Overview of Driver Architecture
===============================
The amd_noc_pmu driver uses a Device Tree-based architecture:

- NoC components (NMU and DDRMC-NSU) are described in Device Tree
- Driver automatically creates performance events based on DT nodes
- Supports optional "label" property for custom event naming
- Uses standard Linux perf PMU framework with delta counting

Device Tree Workflow
====================

Generating Device Tree Files
----------------------------
Run the script with your .rnpi file::

        python3 generate_noc_dt.py design.rnpi

With region name (for multiple NoC instances)::

        python3 generate_noc_dt.py design.rnpi --region fabric

Note: The region name is useful when you have multiple NoC regions
(e.g., fabric, lpd, fpd) or multiple independent NoC configurations
that need to be monitored separately. Each region will create a separate
PMU device (amd-noc-pmu-<region>) with its own set of performance events.

The script automatically:

- Extracts device part number from .rnpi header
- Runs cdoutil to generate versal.txt
- Generates Device Tree files

Expected output::

        Detected device: xcvc1902 (from xcvc1902-vsva2197-2MP-e-S)
        Running cdoutil...
        Successfully generated versal.txt
        Generating Device Tree files...
        Generated amd-noc-pmu.dtsi
        Generated amd-noc-pmu.dtso

With region name (e.g., "fabric"), output files are named accordingly::

        Generated amd-noc-pmu-fabric.dtsi
        Generated amd-noc-pmu-fabric.dtso

Generated Files
^^^^^^^^^^^^^^^
Two files are generated (names adjusted if region name provided):

a) amd-noc-pmu.dtsi (or amd-noc-pmu-<region>.dtsi) - For static DT
   inclusion::

        // SPDX-License-Identifier: GPL-2.0
        // Auto-generated from NoC configuration

        / {
            amd-noc-pmu {               /* or amd-noc-pmu-fabric with region */
                compatible = "amd,noc-pmu";
                #address-cells = <2>;
                #size-cells = <2>;
                ranges;

                nmu@f6000000 {
                    reg = <0x0 0xf6000000 0x0 0x1000>;
                    amd,nmu-id = <0>;
                };

                ddrmc-nsu@f6080000 {
                    reg = <0x0 0xf6080000 0x0 0x1000>;
                    amd,ddrmc-id = <0>;
                    amd,nsu-ports = <0 1>;
                };
            };
        };

b) amd-noc-pmu.dtso (or amd-noc-pmu-<region>.dtso) - For runtime DT
   overlay::

        // SPDX-License-Identifier: GPL-2.0
        // Auto-generated from NoC configuration

        /dts-v1/;
        /plugin/;

        &{/} {
            #address-cells = <2>;
            #size-cells = <2>;

            amd-noc-pmu {               /* or amd-noc-pmu-fabric with region */
                compatible = "amd,noc-pmu";
                #address-cells = <2>;
                #size-cells = <2>;
                ranges;

                /* NMU and DDRMC nodes */
            };
        };

Runtime Device Tree Overlay
---------------------------
For runtime configuration changes, use Device Tree overlay.

1. Compile the overlay::

        dtc -@ -I dts -O dtb -o amd-noc-pmu.dtbo amd-noc-pmu.dtso

   For region-specific files::

        dtc -@ -I dts -O dtb -o amd-noc-pmu-fabric.dtbo amd-noc-pmu-fabric.dtso

   Note: The -@ flag enables symbol generation required for overlays. Recent
   DTC versions can auto-detect overlays based on the /plugin/ directive, but
   using -@ explicitly ensures compatibility with older DTC versions.

Option A: Apply Overlay at U-Boot Stage (Boot-time)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Apply the overlay during boot before Linux starts. This method is useful for
ensuring the overlay is applied early in the boot process.

1. Transfer the compiled overlay (.dtbo) to your TFTP server or boot partition.

2. At U-Boot prompt, load the overlay into memory::

        Versal> tftpb 0x80000 amd-noc-pmu.dtbo

3. Copy the current device tree to a working address::

        Versal> cp $fdtcontroladdr 0x30000000 0x200000

4. Set the working FDT address::

        Versal> fdt addr 0x30000000
        Working FDT set to 0x30000000

5. Resize the FDT to accommodate the overlay::

        Versal> fdt resize 0x20000

6. Apply the overlay::

        Versal> fdt apply 0x80000

7. Verify the overlay was applied (optional)::

        Versal> fdt print

8. Boot Linux with the modified device tree::

        Versal> booti ${kernel_addr} ${ramdisk_addr} 0x30000000

   Note: The FDT address (0x30000000) should be passed to the kernel instead
   of the original $fdtcontroladdr.

Option B: Apply Overlay in Linux (Runtime)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Apply the overlay after Linux has booted. This method allows dynamic loading
and unloading of overlays without rebooting.

1. Transfer the overlay to target system::

        scp amd-noc-pmu.dtbo user@target-system:/tmp/

2. Apply the overlay on target system::

        mkdir -p /sys/kernel/config/device-tree/overlays/noc-pmu
        cat /tmp/amd-noc-pmu.dtbo > /sys/kernel/config/device-tree/overlays/noc-pmu/dtbo

   Note: The overlay file can be placed in any location; /tmp and /lib/firmware
   are just common locations. Use the path where you copied the file.

3. Verify the overlay is applied::

        ls /sys/devices/ | grep amd-noc-pmu

4. To remove the overlay::

        rmdir /sys/kernel/config/device-tree/overlays/noc-pmu

Static Device Tree Inclusion
----------------------------
For production systems, include the Device Tree statically in the kernel build.

1. Copy the generated .dtsi file to your kernel Device Tree directory::

        cp amd-noc-pmu.dtsi <kernel-source>/arch/arm64/boot/dts/xilinx/

   If you used a region name (e.g., "fabric")::

        cp amd-noc-pmu-fabric.dtsi <kernel-source>/arch/arm64/boot/dts/xilinx/

2. Include it in your system Device Tree file (e.g., versal-platform.dts)::

        #include "amd-noc-pmu.dtsi"

   For region-specific files::

        #include "amd-noc-pmu-fabric.dtsi"

3. Rebuild the Device Tree Blob::

        cd <kernel-source>
        make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs

4. Deploy the updated DTB to your target system.

Verify Driver Initialization
============================
After Device Tree is loaded (static or overlay), verify the driver initialized:

1. Verify PMU device exists::

        ls /sys/devices/amd-noc-pmu

2. List format attributes::

        cat /sys/devices/amd-noc-pmu/format/*

   Output should show::

        counter:0-7
        component:8-15
        comp_id:16-23
        event:0-23

Using Performance Monitoring
============================
After Device Tree is loaded and driver initialized, use the Linux perf tool
to monitor NoC performance.

Basic Event Monitoring
----------------------
1. List available NoC events::

        perf list | grep amd-noc-pmu

   Example output (device name varies based on region name used)::

        amd-noc-pmu/nmu_0_read_latency/
        amd-noc-pmu/nmu_0_read_burst/
        amd-noc-pmu/nmu_0_read_byte/
        amd-noc-pmu/nmu_0_write_latency/
        amd-noc-pmu/ddrmc_0_nsu0_read_latency/
        amd-noc-pmu/ddrmc_0_nsu1_write_byte/
        ...

   Note: In the examples below, replace <pmu_device> with your actual device name
   (e.g., amd-noc-pmu, amd-noc-pmu-fabric, etc.)

2. Monitor NMU read latency::

        perf stat -e <pmu_device>/nmu_0_read_latency/ <your_workload>

3. Monitor NMU write bandwidth::

        perf stat -e <pmu_device>/nmu_1_write_byte/ <your_workload>

4. Monitor DDRMC NSU write bursts::

        perf stat -e <pmu_device>/ddrmc_0_nsu0_write_burst/ <your_workload>

Interval Monitoring
-------------------
Use the -I option to get periodic counter updates (useful for tracking
performance over time)::

        perf stat -I 1000 -e <pmu_device>/nmu_0_read_latency/ <workload>

This reports counter deltas every 1000 milliseconds. The driver accumulates
deltas correctly across multiple reads.

Custom Event Names with Labels
------------------------------
You can add optional "label" properties in Device Tree for user-friendly
event names:

1. Edit your Device Tree file to add labels::

        nmu@f6000000 {
            reg = <0x0 0xf6000000 0x0 0x1000>;
            amd,nmu-id = <0>;
            label = "apu";  /* Custom label */
        };

        ddrmc-nsu@f6080000 {
            reg = <0x0 0xf6080000 0x0 0x1000>;
            amd,ddrmc-id = <0>;
            amd,nsu-ports = <0 1>;
            label = "ddr_ctrl";  /* Custom label */
        };

2. After recompiling and loading Device Tree, events use labels::

        perf list | grep amd-noc-pmu

   Output with labels::

        <pmu_device>/apu_0_read_latency/
        <pmu_device>/apu_0_write_byte/
        <pmu_device>/ddr_ctrl_0_nsu0_read_latency/
        <pmu_device>/ddr_ctrl_0_nsu1_write_burst/

3. Use labeled events::

        perf stat -e <pmu_device>/apu_0_read_latency/ <workload>

Traffic Filtering
-----------------
Apply filters to monitor specific traffic patterns using config1 and config2:

1. Filter by AXI ID (monitor only transactions from specific master)::

        perf stat -e '<pmu_device>/nmu_0_read_byte,config1=0x12340001/' <workload>

   Where config1 encoding:

   - Bit 0: Enable AXI ID filter
   - Bits [47:32]: AXI ID value (0x1234 in this example)

2. Filter DDRMC NSU by source ID::

        perf stat -e '<pmu_device>/ddrmc_0_nsu0_read_byte,config1=0x21,config2=0x100/' <workload>

   Where:

   - config1 bit 5: Enable SRC ID filter
   - config2 bits [11:0]: SRC ID value (0x100)

3. Multiple filters (AXI ID + AXLEN range)::

        perf stat -e '<pmu_device>/nmu_0_read_latency,config1=0x080012340003/' <workload>

   Enables AXI ID filter (0x1234) and AXLEN filter (min=0x08)

4. View filter format::

        cat /sys/devices/<pmu_device>/format/*

Event Configuration
-------------------
Each event has a configuration value encoding component type, ID, and counter:

- Bits [7:0]:   Counter type (1=latency, 2=burst, 3=byte for CNT0 read)
- Bits [15:8]:  Component type (1=NMU, 2-5=NSU0-NSU3)
- Bits [23:16]: Component ID

You can specify events directly by config value::

        perf stat -e '<pmu_device>/event=0x060101/' <workload>

This monitors: NMU 6 (0x06), type NMU (0x01), CNT0_LATENCY (0x01)

Available Counter Types
-----------------------
For each component, six counters track different metrics:

Read Counters (CNT0):

- read_latency: Accumulated read transaction latency cycles
- read_burst:   Number of read bursts
- read_byte:    Total bytes read

Write Counters (CNT1):

- write_latency: Accumulated write transaction latency cycles
- write_burst:   Number of write bursts
- write_byte:    Total bytes written
