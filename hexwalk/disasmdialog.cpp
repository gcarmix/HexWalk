#include "disasmdialog.h"
#include "ui_disasmdialog.h"


#include <capstone/capstone.h>
#include <QDebug>

void DisasmDialog::capst(void)
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
    if (cs_open(arch, mode , &handle) != CS_ERR_OK)
        return;
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
DisasmDialog::DisasmDialog(QHexEdit * hexedit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisasmDialog)
{
    ui->setupUi(this);
    _hexedit = hexedit;
}

DisasmDialog::~DisasmDialog()
{
    delete ui;
}

void DisasmDialog::on_btnClose_clicked()
{
    hide();
}


void DisasmDialog::on_btnDisasm_clicked()
{
    capst();
}

void DisasmDialog::refresh()
{
    capst();

}
