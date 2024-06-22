/* ------------------------------------------------------------------------------------------------
 * Copyright (c) 2023. Heqing Huang (feipenghhq@gmail.com)
 *
 * Project: NRC
 * Author: Heqing Huang
 * Date Created: 6/9/2024
 *
 * ------------------------------------------------------------------------------------------------
 * CoreN: Risc V Core Non-pipeline
 * ------------------------------------------------------------------------------------------------
 */

package core

import spinal.core._
import spinal.lib._
import config._
import _root_.misc.SramDpi



case class CoreN(config: RiscCoreConfig) extends Component {
    val ibus = IbusBundle(config)
    val dbus = DbusBundle(config)
    noIoPrefix()

    val uIFU = IFU(config)
    val uIDU = IDU(config)
    val uEXU = EXU(config)

    uIFU.io.branchCtrl <> uEXU.io.branchCtrl
    uIFU.io.ibus <> ibus
    uIFU.io.trapCtrl <> uEXU.io.trapCtrl

    uIDU.io.ifuData <> uIFU.io.ifuData
    uIDU.io.rdWrCtrl <> uEXU.io.rdWrCtrl

    uEXU.io.iduData <> uIDU.io.iduData
    uEXU.io.dbus <> dbus

    val iduData = uIDU.io.iduData.payload

    val uCoreNDPI = CoreNDPI(config)
    uCoreNDPI.io.ebreak := iduData.cpuCtrl.ebreak
    uCoreNDPI.io.ecall := iduData.cpuCtrl.ecall
    uCoreNDPI.io.pc := iduData.pc
    uCoreNDPI.io.data_valid := dbus.valid
    uCoreNDPI.io.data_wen := dbus.write
    uCoreNDPI.io.data_wdata := dbus.wdata
    uCoreNDPI.io.data_addr := dbus.addr
    uCoreNDPI.io.data_wstrb := dbus.strobe
    dbus.rdata := uCoreNDPI.io.data_rdata

    val uIfuSram = SramDpi(config)
    uIfuSram.io.ifetch := True
    uIfuSram.io.pc := uIFU.io.ifuData.payload.pc
    uIfuSram.io.valid := True
    uIfuSram.io.write := False
    uIfuSram.io.addr  <> ibus.addr
    uIfuSram.io.strobe := 0
    uIfuSram.io.rdata <> ibus.data
    uIfuSram.io.wdata := 0
}

object CoreNVerilog extends App {
    val config = RiscCoreConfig(32, 0x80000000L, 32, hasRv32M = true, hasZicsr = true)
    Config.spinal.generateVerilog(CoreN(config)).printPruned()
}