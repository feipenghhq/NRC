/* ------------------------------------------------------------------------------------------------
 * Copyright (c) 2023. Heqing Huang (feipenghhq@gmail.com)
 *
 * Project: NRC
 * Author: Heqing Huang
 * Date Created: 6/6/2024
 *
 * ------------------------------------------------------------------------------------------------
 * IFU: Instruction Fetch Unit
 * ------------------------------------------------------------------------------------------------
 * IFU contains the PC and the logic to access the instruction memory
 * ------------------------------------------------------------------------------------------------
 */

package core

import spinal.core._
import spinal.lib._
import config._
import spinal.core.Verilator.public

case class IfuBundle(config: RiscCoreConfig) extends Bundle {
    val pc = config.xlenUInt
    val instruction = config.xlenBits
}

case class IbusBundle(config: RiscCoreConfig) extends Bundle with IMasterSlave {
    val addr = config.xlenUInt
    val data = config.xlenBits

    override def asMaster(): Unit = {
        out(addr)
        in(data)
    }
}

case class IFU(config: RiscCoreConfig) extends Component {
    val io = new Bundle {
        val ifuData = master Stream(IfuBundle(config))
        val branchCtrl = slave Flow(config.xlenUInt)
        val trapCtrl = slave Flow(config.xlenUInt)
        val ibus = master(IbusBundle(config))
    }
    noIoPrefix()

    val nextPC = config.xlenUInt
    nextPC.addAttribute(public)

    val pc = RegNextWhen(nextPC, io.ifuData.fire) init (config.pcRstVector)
    pc.addAttribute(public)

    val instruction = config.xlenBits
    instruction.addAttribute(public)
    instruction := io.ibus.data

    when(io.trapCtrl.valid) {
        nextPC := io.trapCtrl.payload
    } elsewhen(io.branchCtrl.valid) {
        nextPC := io.branchCtrl.payload
    } otherwise {
        nextPC := pc + 4
    }

    io.ifuData.valid.setAsReg() init False
    when(io.ifuData.ready) {
        io.ifuData.valid := ~io.ifuData.valid
    }

    io.ifuData.pc := pc
    io.ifuData.instruction := instruction

    io.ibus.addr := pc

}

object IFU {
    def apply(config: RiscCoreConfig, ibus: IbusBundle): IFU = {
        val ifu = IFU(config)
        ifu.io.ibus <> ibus
        ifu
    }
}

object IFUVerilog extends App {
    val config = RiscCoreConfig(32, 0x00000000, 32)
    Config.spinal.generateVerilog(IFU(config))
}