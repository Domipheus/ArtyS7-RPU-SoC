
################################################################
# This is a generated script based on design: BRAM_64KB
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2018.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_msg_id "BD_TCL-109" "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source BRAM_64KB_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xc7s50csga324-2
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name BRAM_64KB

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_msg_id "BD_TCL-001" "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_msg_id "BD_TCL-002" "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_msg_id "BD_TCL-003" "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_msg_id "BD_TCL-004" "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_msg_id "BD_TCL-005" "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_msg_id "BD_TCL-114" "ERROR" $errMsg}
   return $nRet
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_msg_id "BD_TCL-100" "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_msg_id "BD_TCL-101" "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports

  # Create ports
  set I_addra [ create_bd_port -dir I -from 31 -to 0 I_addra ]
  set I_addrb [ create_bd_port -dir I -from 31 -to 0 I_addrb ]
  set I_clka [ create_bd_port -dir I -type clk I_clka ]
  set I_clkb [ create_bd_port -dir I -type clk I_clkb ]
  set I_dina [ create_bd_port -dir I -from 31 -to 0 I_dina ]
  set I_dinb [ create_bd_port -dir I -from 31 -to 0 I_dinb ]
  set I_ena [ create_bd_port -dir I I_ena ]
  set I_enb [ create_bd_port -dir I I_enb ]
  set I_wea [ create_bd_port -dir I -from 3 -to 0 I_wea ]
  set I_web [ create_bd_port -dir I -from 3 -to 0 I_web ]
  set O_douta [ create_bd_port -dir O -from 31 -to 0 O_douta ]
  set O_doutb [ create_bd_port -dir O -from 31 -to 0 O_doutb ]

  # Create instance: blk_mem_gen_0, and set properties
  set blk_mem_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_0 ]
  set_property -dict [ list \
   CONFIG.Byte_Size {8} \
   CONFIG.Coe_File {../../../../imports/RPU/vivado/rom/coe/bootloader.coe} \
   CONFIG.EN_SAFETY_CKT {false} \
   CONFIG.Enable_32bit_Address {true} \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Fill_Remaining_Memory_Locations {true} \
   CONFIG.Load_Init_File {true} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Register_PortA_Output_of_Memory_Primitives {false} \
   CONFIG.Register_PortB_Output_of_Memory_Primitives {false} \
   CONFIG.Use_Byte_Write_Enable {true} \
   CONFIG.Use_RSTA_Pin {false} \
   CONFIG.Use_RSTB_Pin {false} \
   CONFIG.Write_Depth_A {16384} \
   CONFIG.use_bram_block {Stand_Alone} \
 ] $blk_mem_gen_0

  # Create port connections
  connect_bd_net -net I_addra_1 [get_bd_ports I_addra] [get_bd_pins blk_mem_gen_0/addra]
  connect_bd_net -net I_addrb_1 [get_bd_ports I_addrb] [get_bd_pins blk_mem_gen_0/addrb]
  connect_bd_net -net I_clka_1 [get_bd_ports I_clka] [get_bd_pins blk_mem_gen_0/clka]
  connect_bd_net -net I_clkb_1 [get_bd_ports I_clkb] [get_bd_pins blk_mem_gen_0/clkb]
  connect_bd_net -net I_dina_1 [get_bd_ports I_dina] [get_bd_pins blk_mem_gen_0/dina]
  connect_bd_net -net I_dinb_1 [get_bd_ports I_dinb] [get_bd_pins blk_mem_gen_0/dinb]
  connect_bd_net -net I_ena_1 [get_bd_ports I_ena] [get_bd_pins blk_mem_gen_0/ena]
  connect_bd_net -net I_enb_1 [get_bd_ports I_enb] [get_bd_pins blk_mem_gen_0/enb]
  connect_bd_net -net I_wea_1 [get_bd_ports I_wea] [get_bd_pins blk_mem_gen_0/wea]
  connect_bd_net -net I_web_1 [get_bd_ports I_web] [get_bd_pins blk_mem_gen_0/web]
  connect_bd_net -net blk_mem_gen_0_douta [get_bd_ports O_douta] [get_bd_pins blk_mem_gen_0/douta]
  connect_bd_net -net blk_mem_gen_0_doutb [get_bd_ports O_doutb] [get_bd_pins blk_mem_gen_0/doutb]

  # Create address segments


  # Restore current instance
  current_bd_instance $oldCurInst

  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


