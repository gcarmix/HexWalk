/*
 * Copyright (C) 2025 Carmix <carmixdev@gmail.com>
 *
 * This file is part of HexWalk.
 *
 * HexWalk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HexWalk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "disasmwidget.h"
#include "ui_disasmwidget.h"
#include <capstone/capstone.h>
void DisasmWidget::capst(void)
{
    csh handle;
    cs_insn *insn;
    size_t count;
    QString message;
    ui->lstDisasm->clear();
    QByteArray selectedHex = _hexedit->selectedDataBa();
    qint64 addrBase = _hexedit->cursorPosition()/2 - selectedHex.size();
    ui->edtBaseAddr->setText(QString("%1").arg(addrBase,2,16,QLatin1Char('0')).toUpper());
    ui->edtSelected->setText(QString("%1").arg(selectedHex.size()));
    cs_arch arch = CS_ARCH_X86;
    uint32_t mode_int = CS_MODE_64;
    cs_mode mode = CS_MODE_64;
    uint32_t endianness = CS_MODE_LITTLE_ENDIAN;
    if(ui->comboEndian->currentText().contains("Little"))
    {
        endianness = CS_MODE_LITTLE_ENDIAN;
    }
    else
    {
        endianness = CS_MODE_BIG_ENDIAN;
    }
    if(ui->comboBox->currentText().contains("x86/64"))
    {
        arch = CS_ARCH_X86;
        mode_int = CS_MODE_64;
    }
    else if(ui->comboBox->currentText().contains("x86/32"))
    {
        arch = CS_ARCH_X86;
        mode_int = CS_MODE_32;
    }
    else if(ui->comboBox->currentText() == "ARM32")
    {
        arch = CS_ARCH_ARM;
        mode_int = CS_MODE_ARM;
    }
    else if(ui->comboBox->currentText() == "ARM32/THUMB")
    {
        arch = CS_ARCH_ARM;
        mode_int = CS_MODE_THUMB;
    }
    else if(ui->comboBox->currentText().contains("ARM64"))
    {
        arch = CS_ARCH_ARM64;
        mode_int = CS_MODE_ARM;
    }
    else if(ui->comboBox->currentText().contains("MIPS"))
    {
        arch = CS_ARCH_MIPS;
        mode_int = CS_MODE_MIPS32;
    }
    mode = (cs_mode) (mode_int | endianness);
    if (cs_err ret = cs_open(arch, mode , &handle))
    {
        message.sprintf("Invalid mode (e.g. x86 can't be Big Endian)");
        ui->lstDisasm->addItem(message);
        return;
    }
    count = cs_disasm(handle, (const uint8_t*)selectedHex.data(), selectedHex.size(), addrBase, 0, &insn);
    if (count > 0) {
        size_t j;
        for (j = 0; j < count; j++) {
            message.sprintf("0x%llX: %s %s", insn[j].address, insn[j].mnemonic,
                            insn[j].op_str);
            ui->lstDisasm->addItem(message);

        }

        cs_free(insn, count);
    } else
    {

        message.sprintf("ERROR: Failed to disassemble given code!\n");
        ui->lstDisasm->addItem(message);
    }
    cs_close(&handle);

}
DisasmWidget::DisasmWidget(QHexEdit * hexedit,QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DisasmWidget)
{
    ui->setupUi(this);
    _hexedit = hexedit;
    hide();
}

DisasmWidget::~DisasmWidget()
{
    delete ui;
}




void DisasmWidget::on_btnDisasm_clicked()
{
    capst();
}

void DisasmWidget::refresh()
{
    capst();

}
