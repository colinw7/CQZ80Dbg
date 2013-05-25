#include <CQZ80Dbg.h>
#include <CZ80.h>
#include <CStrUtil.h>

#include <QVBoxLayout>
#include <QTextEdit>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QScrollBar>
#include <QMouseEvent>
#include <cassert>

using std::string;

CQZ80Dbg::
CQZ80Dbg(CZ80 *z80) :
 CZ80Trace(*z80), z80_(z80)
{
  z80_->addTrace(this);

  addWidgets();

  setMemoryText();

  setInstructionsText();

  setStackText();

  setBreakpointText();

  regChanged(CZ80_REG_NONE);
}

void
CQZ80Dbg::
addWidgets()
{
  fixed_font_ = QFont("Courier", 10);

  QFontMetrics fm(fixed_font_);

  int char_height = fm.height();

  //----

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  setWindowTitle("Z80 Emulator (Debug)");

  QWidget *top_frame    = new QWidget;
  QWidget *bottom_frame = new QWidget;

  QHBoxLayout *top_layout    = new QHBoxLayout(top_frame);
  QVBoxLayout *bottom_layout = new QVBoxLayout(bottom_frame);

  top_layout   ->setMargin(2); top_layout   ->setSpacing(2);
  bottom_layout->setMargin(2); bottom_layout->setSpacing(2);

  layout->addWidget(top_frame);
  layout->addWidget(bottom_frame);

  //----

  QWidget *left_frame  = new QWidget;
  QWidget *right_frame = new QWidget;

  top_layout->addWidget(left_frame);
  top_layout->addWidget(right_frame);

  QVBoxLayout *left_layout  = new QVBoxLayout(left_frame );
  QVBoxLayout *right_layout = new QVBoxLayout(right_frame);

  left_layout ->setMargin(2); left_layout ->setSpacing(2);
  right_layout->setMargin(2); right_layout->setSpacing(2);

  //----

  int memory_width = fm.width("0000  00 00 00 00 00 00 00 00  XXXXXXXX");

  QWidget *memory_frame = new QGroupBox("Memory");

  QHBoxLayout *memory_layout = new QHBoxLayout(memory_frame);

  memory_text_ = new CQZ80Mem(this);

  memory_text_->setFont(fixed_font_);

  memory_text_->setFixedWidth (memory_width + 32);
  memory_text_->setFixedHeight(char_height*20);

  memory_vbar_ = new QScrollBar;

  memory_vbar_->setPageStep  (20);
  memory_vbar_->setSingleStep(1);
  memory_vbar_->setRange     (0, 8192 - memory_vbar_->pageStep());

  connect(memory_vbar_, SIGNAL(valueChanged(int)),
          memory_text_, SLOT(sliderSlot(int)));

  memory_layout->addWidget(memory_text_);
  memory_layout->addWidget(memory_vbar_);
  memory_layout->addStretch();

  left_layout->addWidget(memory_frame);

  //--

  int instructions_width = fm.width("0000  123456789012  AAAAAAAAAAAAAAAAAA");

  QWidget *instructions_frame = new QGroupBox("Instructions");

  QHBoxLayout *instructions_layout = new QHBoxLayout(instructions_frame);

  instructions_text_ = new CQZ80Inst(this);

  instructions_text_->setFont(fixed_font_);

  instructions_text_->setFixedWidth(instructions_width + 32);
  instructions_text_->setFixedHeight(char_height*20);

  instructions_vbar_ = new QScrollBar;

  instructions_vbar_->setPageStep  (20);
  instructions_vbar_->setSingleStep(1);
  instructions_vbar_->setRange     (0, 8192 - instructions_vbar_->pageStep());

  connect(instructions_vbar_, SIGNAL(valueChanged(int)),
          instructions_text_, SLOT(sliderSlot(int)));

  instructions_layout->addWidget(instructions_text_);
  instructions_layout->addWidget(instructions_vbar_);
  instructions_layout->addStretch();

  left_layout->addWidget(instructions_frame);

  op_data_ = new QLineEdit;

  op_data_->setReadOnly(true);

  left_layout->addWidget(op_data_);

  //----

  QWidget *registers_frame = new QGroupBox("Registers");

  QGridLayout *registers_layout = new QGridLayout(registers_frame);

  af_edit_  = new CQZ80RegEdit(this, CZ80_REG_AF );
  af1_edit_ = new CQZ80RegEdit(this, CZ80_REG_AF1);
  bc_edit_  = new CQZ80RegEdit(this, CZ80_REG_BC );
  bc1_edit_ = new CQZ80RegEdit(this, CZ80_REG_BC1);
  de_edit_  = new CQZ80RegEdit(this, CZ80_REG_DE );
  de1_edit_ = new CQZ80RegEdit(this, CZ80_REG_DE1);
  hl_edit_  = new CQZ80RegEdit(this, CZ80_REG_HL );
  hl1_edit_ = new CQZ80RegEdit(this, CZ80_REG_HL1);
  ix_edit_  = new CQZ80RegEdit(this, CZ80_REG_IX );
  i_edit_   = new CQZ80RegEdit(this, CZ80_REG_I  );
  iy_edit_  = new CQZ80RegEdit(this, CZ80_REG_IY );
  r_edit_   = new CQZ80RegEdit(this, CZ80_REG_R  );
  sp_edit_  = new CQZ80RegEdit(this, CZ80_REG_SP );
  iff_edit_ = new CQZ80RegEdit(this, CZ80_REG_IFF);
  pc_edit_  = new CQZ80RegEdit(this, CZ80_REG_PC );

  registers_layout->addWidget(af_edit_ , 0, 0);
  registers_layout->addWidget(af1_edit_, 0, 1);

  registers_layout->addWidget(bc_edit_ , 1, 0);
  registers_layout->addWidget(bc1_edit_, 1, 1);

  registers_layout->addWidget(de_edit_ , 2, 0);
  registers_layout->addWidget(de1_edit_, 2, 1);

  registers_layout->addWidget(hl_edit_ , 3, 0);
  registers_layout->addWidget(hl1_edit_, 3, 1);

  registers_layout->addWidget(ix_edit_ , 4, 0);
  registers_layout->addWidget(i_edit_  , 4, 1);

  registers_layout->addWidget(iy_edit_ , 5, 0);
  registers_layout->addWidget(r_edit_  , 5, 1);

  registers_layout->addWidget(sp_edit_ , 6, 0);
  registers_layout->addWidget(iff_edit_, 6, 1);

  registers_layout->addWidget(pc_edit_ , 7, 0);

  registers_layout->setColumnStretch(2, 1);

  right_layout->addWidget(registers_frame);

  //--

  QWidget *flags_frame = new QGroupBox("Flags");

  QGridLayout *flags_layout = new QGridLayout(flags_frame);

  flags_layout->setSpacing(6);

  s_flag_checkbox_ = new QCheckBox("");
  z_flag_checkbox_ = new QCheckBox("");
  y_flag_checkbox_ = new QCheckBox("");
  h_flag_checkbox_ = new QCheckBox("");
  x_flag_checkbox_ = new QCheckBox("");
  p_flag_checkbox_ = new QCheckBox("");
  n_flag_checkbox_ = new QCheckBox("");
  c_flag_checkbox_ = new QCheckBox("");

  flags_layout->addWidget(new QLabel("S"), 0, 0); flags_layout->addWidget(s_flag_checkbox_, 1, 0);
  flags_layout->addWidget(new QLabel("Z"), 0, 1); flags_layout->addWidget(z_flag_checkbox_, 1, 1);
  flags_layout->addWidget(new QLabel("Y"), 0, 2); flags_layout->addWidget(y_flag_checkbox_, 1, 2);
  flags_layout->addWidget(new QLabel("H"), 0, 3); flags_layout->addWidget(h_flag_checkbox_, 1, 3);
  flags_layout->addWidget(new QLabel("X"), 0, 4); flags_layout->addWidget(x_flag_checkbox_, 1, 4);
  flags_layout->addWidget(new QLabel("P"), 0, 5); flags_layout->addWidget(p_flag_checkbox_, 1, 5);
  flags_layout->addWidget(new QLabel("N"), 0, 6); flags_layout->addWidget(n_flag_checkbox_, 1, 6);
  flags_layout->addWidget(new QLabel("C"), 0, 7); flags_layout->addWidget(c_flag_checkbox_, 1, 7);

  flags_layout->setColumnStretch(8, 1);

  right_layout->addWidget(flags_frame);

  //--

  QWidget *stack_frame = new QGroupBox("Stack");

  QVBoxLayout *stack_layout = new QVBoxLayout(stack_frame);

  stack_text_ = new QTextEdit;

  stack_text_->setReadOnly(true);

  stack_text_->setFont(fixed_font_);

  stack_layout->addWidget(stack_text_);

  right_layout->addWidget(stack_frame);

  //--

  QWidget *breakpoints_frame = new QGroupBox("Breakpoints");

  QVBoxLayout *breakpoints_layout = new QVBoxLayout(breakpoints_frame);

  breakpoints_text_ = new QTextEdit;

  breakpoints_text_->setReadOnly(true);

  breakpoints_text_->setFont(fixed_font_);

  breakpoints_layout->addWidget(breakpoints_text_);

  QWidget *breakpoint_toolbar = new QWidget;

  QHBoxLayout *breakpoint_toolbar_layout = new QHBoxLayout(breakpoint_toolbar);

  QPushButton *addBreakpointButton    = new QPushButton("Add"   );
  QPushButton *deleteBreakpointButton = new QPushButton("Delete");
  QPushButton *clearBreakpointButton  = new QPushButton("Clear" );

  connect(addBreakpointButton   , SIGNAL(clicked()), this, SLOT(addBreakpointSlot   ()));
  connect(deleteBreakpointButton, SIGNAL(clicked()), this, SLOT(deleteBreakpointSlot()));
  connect(clearBreakpointButton , SIGNAL(clicked()), this, SLOT(clearBreakpointSlot ()));

  breakpoint_toolbar_layout->addWidget(addBreakpointButton);
  breakpoint_toolbar_layout->addWidget(deleteBreakpointButton);
  breakpoint_toolbar_layout->addWidget(clearBreakpointButton);

  breakpoints_layout->addWidget(breakpoint_toolbar);

  right_layout->addWidget(breakpoints_frame);

  //-----

  QCheckBox *trace_checkbox = new QCheckBox("Trace");

  bottom_layout->addWidget(trace_checkbox);

  connect(trace_checkbox, SIGNAL(stateChanged(int)), this, SLOT(setTraceSlot(int)));

  QWidget *buttons_toolbar = new QWidget;

  QHBoxLayout *buttons_toolbar_layout = new QHBoxLayout(buttons_toolbar);

  QPushButton *run_button      = new QPushButton("Run");
  QPushButton *next_button     = new QPushButton("Next");
  QPushButton *step_button     = new QPushButton("Step");
  QPushButton *continue_button = new QPushButton("Continue");
  QPushButton *stop_button     = new QPushButton("Stop");
  QPushButton *restart_button  = new QPushButton("Restart");
  QPushButton *exit_button     = new QPushButton("Exit");

  connect(run_button     , SIGNAL(clicked()), this, SLOT(runSlot()));
  connect(next_button    , SIGNAL(clicked()), this, SLOT(nextSlot()));
  connect(step_button    , SIGNAL(clicked()), this, SLOT(stepSlot()));
  connect(continue_button, SIGNAL(clicked()), this, SLOT(continueSlot()));
  connect(stop_button    , SIGNAL(clicked()), this, SLOT(stopSlot()));
  connect(restart_button , SIGNAL(clicked()), this, SLOT(restartSlot()));
  connect(exit_button    , SIGNAL(clicked()), this, SLOT(exitSlot()));

  buttons_toolbar_layout->addWidget(run_button);
  buttons_toolbar_layout->addWidget(next_button);
  buttons_toolbar_layout->addWidget(step_button);
  buttons_toolbar_layout->addWidget(continue_button);
  buttons_toolbar_layout->addWidget(stop_button);
  buttons_toolbar_layout->addWidget(restart_button);
  buttons_toolbar_layout->addWidget(exit_button);

  bottom_layout->addWidget(buttons_toolbar);
}

void
CQZ80Dbg::
setMemoryText()
{
  uint len = 65536;

  ushort num_lines = len / 8;

  if ((len % 8) != 0) ++num_lines;

  uint pos = z80_->getPC();

  z80_->setPC(0);

  string str;

  uint pos1 = 0;

  for (ushort i = 0; i < num_lines; ++i) {
    setMemoryLine(pos1);

    pos1 += 8;
  }

  z80_->setPC(pos);
}

void
CQZ80Dbg::
setMemoryLine(uint pos)
{
  string pcStr = CStrUtil::toHexString(pos, 4);

  //-----

  string memStr;

  for (ushort j = 0; j < 8; ++j) {
    if (j > 0) memStr += " ";

    memStr += CStrUtil::toHexString(z80_->getByte(pos + j), 2);
  }

  string textStr;

  for (ushort j = 0; j < 8; ++j) {
    uchar c = z80_->getByte(pos + j);

    textStr += getByteChar(c);
  }

  memory_text_->setLine(pos, pcStr, memStr, textStr);
}

string
CQZ80Dbg::
getByteChar(uchar c)
{
  string str;

  if (c >= 0x20 && c < 0x7f)
    str += c;
  else
    str += '.';

  return str;
}

void
CQZ80Dbg::
setInstructionsText()
{
  uint init_pc = z80_->getPC();

  uint pos1 = 0;
  uint pos2 = 65536;

  instructions_text_->clear();

  uint pc       = pos1;
  bool pc_found = false;

  while (pc < pos2) {
    if (! pc_found && pc >= init_pc) {
      pc       = init_pc;
      pc_found = true;
    }

    //-----

    string pcStr = CStrUtil::toHexString(pc, 4);

    //-----

    uint last_pc = pc;

    z80_->setPC(pc);

    CZ80OpData op_data;

    z80_->readOpData(&op_data);

    pc = z80_->getPC();

    if (pc < last_pc) pc = pos2;

    //-----

    string codeStr;

    ushort len1 = 0;

    for (uint i = last_pc; i < pc; ++i) {
      if (i > last_pc) codeStr += " ";

      codeStr += CStrUtil::toHexString(z80_->getByte(i), 2);

      len1 += 3;
    }

    for ( ; len1 < 12; ++len1)
      codeStr += " ";

    //-----

    string textStr = "; ";

    if (op_data.op != NULL)
      textStr += op_data.getOpString();
    else
      textStr += "??";

    instructions_text_->setLine(last_pc, pcStr, codeStr, textStr);
  }

  uint numLines = instructions_text_->getNumLines();

  instructions_vbar_->setRange(0, numLines - instructions_vbar_->pageStep());

  instructions_vbar_->setValue(0);

  z80_->setPC(init_pc);

  instructions_text_->update();
}

void
CQZ80Dbg::
setStackText()
{
  stack_text_->clear();

  ushort sp = z80_->getSP();

  ushort sp1 = sp - 4;

  string str;

  for (ushort i = 0; i < 16; ++i) {
    ushort sp2 = sp1 + i;

    str = "";

    if (sp2 == sp)
      str += "<b><font color=\"red\">&gt;</font></b>";
    else
      str += " ";

    str += CStrUtil::toHexString(sp2, 4);

    str += " ";

    str += CStrUtil::toHexString(z80_->getByte(sp2), 2);

    stack_text_->append(str.c_str());
  }
}

void
CQZ80Dbg::
setBreakpointText()
{
  breakpoints_text_->clear();

  instructions_text_->clearBreakpoints();

  //----

  std::vector<ushort> addrs;

  z80_->getBreakpoints(addrs);

  string str;

  for (uint i = 0; i < addrs.size(); ++i) {
    str = CStrUtil::toHexString(addrs[i], 4);

    breakpoints_text_->append(str.c_str());

    instructions_text_->addBreakPoint(addrs[i]);
  }
}

void
CQZ80Dbg::
postStepProc()
{
}

void
CQZ80Dbg::
regChanged(CZ80Reg reg)
{
  //if (! debug_ || ! follow_) return;

  if (reg == CZ80_REG_AF  || reg == CZ80_REG_NONE) {
    af_edit_->setValue(z80_->getAF());

    c_flag_checkbox_->setChecked(z80_->tstCFlag());
    n_flag_checkbox_->setChecked(z80_->tstNFlag());
    p_flag_checkbox_->setChecked(z80_->tstPFlag());
    x_flag_checkbox_->setChecked(z80_->tstXFlag());
    h_flag_checkbox_->setChecked(z80_->tstHFlag());
    y_flag_checkbox_->setChecked(z80_->tstYFlag());
    z_flag_checkbox_->setChecked(z80_->tstZFlag());
    s_flag_checkbox_->setChecked(z80_->tstSFlag());
  }
  if (reg == CZ80_REG_BC  || reg == CZ80_REG_NONE)
    bc_edit_->setValue(z80_->getBC());
  if (reg == CZ80_REG_DE  || reg == CZ80_REG_NONE)
    de_edit_->setValue(z80_->getDE());
  if (reg == CZ80_REG_HL  || reg == CZ80_REG_NONE)
    hl_edit_->setValue(z80_->getHL());
  if (reg == CZ80_REG_IX  || reg == CZ80_REG_NONE)
    ix_edit_->setValue(z80_->getIX());
  if (reg == CZ80_REG_IY  || reg == CZ80_REG_NONE)
    iy_edit_->setValue(z80_->getIY());
  if (reg == CZ80_REG_SP  || reg == CZ80_REG_NONE) {
    sp_edit_->setValue(z80_->getSP());

    //setStackText();
  }

  if (reg == CZ80_REG_PC || reg == CZ80_REG_NONE) {
    uint pc = z80_->getPC();

    pc_edit_->setValue(pc);

    //----

    int mem1 = memory_vbar_->value();
    int mem2 = mem1 + 20;
    int mem  = pc / 8;

    if (mem < mem1 || mem > mem2)
      memory_vbar_->setValue(mem);
    else
      memory_text_->update();

    //----

    uint line_num;

    if (! instructions_text_->getLineForPC(pc, line_num))
      setInstructionsText();

    if (instructions_text_->getLineForPC(pc, line_num))
      instructions_vbar_->setValue(line_num);

    //----

    CZ80OpData op_data;

    z80_->readOpData(&op_data);

    if (op_data.op != NULL)
      op_data_->setText(op_data.getOpString().c_str());
    else
      op_data_->setText("");

    z80_->setPC(pc);
  }

  if (reg == CZ80_REG_I   || reg == CZ80_REG_NONE)
    i_edit_  ->setValue(z80_->getI  ());
  if (reg == CZ80_REG_R   || reg == CZ80_REG_NONE)
    r_edit_  ->setValue(z80_->getR  ());
  if (reg == CZ80_REG_AF1 || reg == CZ80_REG_NONE)
    af1_edit_->setValue(z80_->getAF1());
  if (reg == CZ80_REG_BC1 || reg == CZ80_REG_NONE)
    bc1_edit_->setValue(z80_->getBC1());
  if (reg == CZ80_REG_DE1 || reg == CZ80_REG_NONE)
    de1_edit_->setValue(z80_->getDE1());
  if (reg == CZ80_REG_HL1 || reg == CZ80_REG_NONE)
    hl1_edit_->setValue(z80_->getHL1());
  if (reg == CZ80_REG_IFF || reg == CZ80_REG_NONE)
    iff_edit_->setValue(z80_->getIFF());
}

void
CQZ80Dbg::
memChanged(ushort pos, ushort len)
{
  //if (! debug_ || ! follow_) return;

  ushort pos1 = pos;
  ushort pos2 = pos + len;

  uint line_num1 = pos1/8;
  uint line_num2 = pos2/8;

  for (uint line_num = line_num1; line_num <= line_num2; ++line_num)
    setMemoryLine(8*line_num);
}

void
CQZ80Dbg::
breakpointsChanged()
{
  setBreakpointText();
}

QLineEdit *
CQZ80Dbg::
createRegisterEdit()
{
  QLineEdit *edit = new QLineEdit;

  edit->setFont(fixed_font_);

  QFontMetrics fm(fixed_font_);

  edit->setFixedWidth(fm.width("0000") + 16);

  return edit;
}

void
CQZ80Dbg::
addBreakpointSlot()
{
  if (! z80_->isBreakpoint(z80_->getPC()))
    z80_->addBreakpoint(z80_->getPC());
}

void
CQZ80Dbg::
deleteBreakpointSlot()
{
  if (z80_->isBreakpoint(z80_->getPC()))
    z80_->removeBreakpoint(z80_->getPC());
}

void
CQZ80Dbg::
clearBreakpointSlot()
{
  z80_->removeAllBreakpoints();
}

void
CQZ80Dbg::
setTraceSlot(int trace)
{
  follow_ = trace;
}

void
CQZ80Dbg::
runSlot()
{
  z80_->execute();
}

void
CQZ80Dbg::
nextSlot()
{
  z80_->next();
}

void
CQZ80Dbg::
stepSlot()
{
  z80_->step();
}

void
CQZ80Dbg::
continueSlot()
{
  z80_->cont();
}

void
CQZ80Dbg::
stopSlot()
{
  z80_->stop();
}

void
CQZ80Dbg::
restartSlot()
{
  z80_->reset();

  z80_->setPC(z80_->getLoadPos());
}

void
CQZ80Dbg::
exitSlot()
{
  exit(0);
}

//-----------

CQZ80Mem::
CQZ80Mem(CQZ80Dbg *dbg) :
 QWidget(NULL), dbg_(dbg), y_offset_(0), dx_(2)
{
  lines_.resize(8192);
}

void
CQZ80Mem::
setLine(uint pc, const std::string &pcStr, const std::string &memStr,
        const std::string &textStr)
{
  uint line_num = pc / 8;

  lines_[line_num] = CQZ80MemLine(pcStr, memStr, textStr);
}

void
CQZ80Mem::
paintEvent(QPaintEvent *)
{
  uint pc = dbg_->getZ80()->getPC();

  QPainter p(this);

  p.fillRect(rect(), QBrush(QColor(255,255,255)));

  QFontMetrics fm(font());

  char_height_ = fm.height();
  char_width_  = fm.width(" ");

  int char_ascent = fm.ascent();

  int w1 =  4*char_width_;
  int w2 = 23*char_width_;

  int y = -y_offset_*char_height_ + char_ascent;

  int ymin = -char_height_;
  int ymax = height() + char_height_;

  uint numLines = lines_.size();

  for (uint i = 0; i < numLines; ++i) {
    if (y >= ymin && y <= ymax) {
      int x = dx_;

      const CQZ80MemLine &line = lines_[i];

      p.setPen(QColor(0,0,220));

      p.drawText(x, y, line.pcStr.c_str());

      x += w1 + char_width_;

      p.setPen(QColor(0,0,0));

      uint pc1 = 8*i;
      uint pc2 = pc1 + 8;

      if (pc >= pc1 && pc < pc2) {
        int i1 = 3*(pc - pc1);
        int i2 = i1 + 2;

        string lhs = line.memStr.substr(0, i1);
        string mid = line.memStr.substr(i1, 2);
        string rhs = line.memStr.substr(i2);

        p.drawText(x                 , y, lhs.c_str());
        p.drawText(x + char_width_*i2, y, rhs.c_str());

        p.setPen(QColor(255,0,0));

        p.drawText(x + char_width_*i1, y, mid.c_str());
      }
      else {
        p.drawText(x, y, line.memStr.c_str());
      }

      x += w2 + char_width_;

      p.setPen(QColor(0,220,0));

      p.drawText(x, y, line.textStr.c_str());
    }

    y += char_height_;
  }
}

void
CQZ80Mem::
mouseDoubleClickEvent(QMouseEvent *e)
{
  int ix = (e->pos().x() - dx_                   )/char_width_ ;
  int iy = (e->pos().y() + y_offset_*char_height_)/char_height_;

  if (ix < 4 || ix >= 28  ) return;
  if (iy < 0 || iy >= 8192) return;

  uint pc = int((ix - 4)/3) + iy*8;

  dbg_->getZ80()->setPC(pc);

  dbg_->regChanged(CZ80_REG_PC);
}

void
CQZ80Mem::
sliderSlot(int y)
{
  y_offset_ = y;

  update();
}

//-----------

CQZ80Inst::
CQZ80Inst(CQZ80Dbg *dbg) :
 QWidget(NULL), dbg_(dbg), y_offset_(0)
{
  line_num_ = 0;

  lines_.resize(65536);
}

void
CQZ80Inst::
clear()
{
  line_num_ = 0;

  pc_line_map_.clear();
  line_pc_map_.clear();
}

void
CQZ80Inst::
setLine(uint pc, const std::string &pcStr, const std::string &codeStr,
        const std::string &textStr)
{
  lines_[line_num_] = CQZ80InstLine(pc, pcStr, codeStr, textStr);

  pc_line_map_[pc       ] = line_num_;
  line_pc_map_[line_num_] = pc;

  ++line_num_;
}

bool
CQZ80Inst::
getLineForPC(uint pc, uint &line_num) const
{
  PCLineMap::const_iterator p = pc_line_map_.find(pc);

  if (p == pc_line_map_.end())
    return false;

  line_num = (*p).second;

  return true;
}

uint
CQZ80Inst::
getPCForLine(uint line_num)
{
  return line_pc_map_[line_num];
}

void
CQZ80Inst::
paintEvent(QPaintEvent *)
{
  uint pc = dbg_->getZ80()->getPC();

  QPainter p(this);

  p.fillRect(rect(), QBrush(QColor(255,255,255)));

  QFontMetrics fm(font());

  char_height_ = fm.height();

  int char_width  = fm.width(" ");
  int char_ascent = fm.ascent();

  int w1 =  4*char_width;
  int w2 = 12*char_width;

  int y = -y_offset_*char_height_ + char_ascent;

  int ymin = -char_height_;
  int ymax = height() + char_height_;

  uint numLines = line_num_;

  for (uint i = 0; i < numLines; ++i) {
    if (y >= ymin && y <= ymax) {
      const CQZ80InstLine &line = lines_[i];

      int x = 2;

      if (line.pc == pc) {
        p.setPen(QColor(255,0,0));

        p.drawText(x, y, ">");
      }

      x += char_width;

      p.setPen(QColor(0,0,220));

      p.drawText(x, y, line.pcStr.c_str());

      x += w1 + char_width;

      p.setPen(QColor(0,0,0));

      p.drawText(x, y, line.codeStr.c_str());

      x += w2 + char_width;

      p.setPen(QColor(0,220,0));

      p.drawText(x, y, line.textStr.c_str());
    }

    y += char_height_;
  }
}

void
CQZ80Inst::
mouseDoubleClickEvent(QMouseEvent *e)
{
  int iy = (e->pos().y() + y_offset_*char_height_)/char_height_;

  dbg_->getZ80()->setPC(getPCForLine(iy));

  dbg_->regChanged(CZ80_REG_PC);
}

void
CQZ80Inst::
sliderSlot(int y)
{
  y_offset_ = y;

  update();
}

//------

CQZ80RegEdit::
CQZ80RegEdit(CQZ80Dbg *dbg, CZ80Reg reg) :
 QWidget(NULL), dbg_(dbg), reg_(reg)
{
  QHBoxLayout *layout = new QHBoxLayout(this);

  layout->setMargin(2); layout->setMargin(2);

  QString str;

  switch (reg) {
    case CZ80_REG_AF : str = "AF" ; break;
    case CZ80_REG_AF1: str = "AF'"; break;
    case CZ80_REG_BC : str = "BC" ; break;
    case CZ80_REG_BC1: str = "BC'"; break;
    case CZ80_REG_DE : str = "DE" ; break;
    case CZ80_REG_DE1: str = "DE'"; break;
    case CZ80_REG_HL : str = "HL" ; break;
    case CZ80_REG_HL1: str = "HL'"; break;
    case CZ80_REG_IX : str = "IX" ; break;
    case CZ80_REG_I  : str = "I"  ; break;
    case CZ80_REG_IY : str = "IY" ; break;
    case CZ80_REG_R  : str = "R"  ; break;
    case CZ80_REG_SP : str = "SP" ; break;
    case CZ80_REG_PC : str = "PC" ; break;
    case CZ80_REG_IFF: str = "IFF"; break;
    default          : assert(false);
  }

  QFontMetrics fm(font());

  QLabel *label = new QLabel(str);

  label->setFixedWidth(fm.width("XX'") + 4);

  edit_ = new QLineEdit;

  edit_->setFont(dbg_->getFixedFont());

  layout->addWidget(label);
  layout->addWidget(edit_);

  edit_->setFixedWidth(fm.width("0000") + 16);

  connect(edit_, SIGNAL(returnPressed()), this, SLOT(valueChangedSlot()));
}

void
CQZ80RegEdit::
setValue(uint value)
{
  int len = 4;

  if (reg_ == CZ80_REG_I || reg_ == CZ80_REG_R || reg_ == CZ80_REG_IFF) len = 2;

  edit_->setText(CStrUtil::toHexString(value, len).c_str());
}

void
CQZ80RegEdit::
valueChangedSlot()
{
  uint value;

  if (! CStrUtil::decodeHexString(edit_->text().toStdString(), &value))
    return;

  switch (reg_) {
    case CZ80_REG_AF : dbg_->getZ80()->setAF (value); break;
    case CZ80_REG_AF1: dbg_->getZ80()->setAF1(value); break;
    case CZ80_REG_BC : dbg_->getZ80()->setBC (value); break;
    case CZ80_REG_BC1: dbg_->getZ80()->setBC1(value); break;
    case CZ80_REG_DE : dbg_->getZ80()->setDE (value); break;
    case CZ80_REG_DE1: dbg_->getZ80()->setDE1(value); break;
    case CZ80_REG_HL : dbg_->getZ80()->setHL (value); break;
    case CZ80_REG_HL1: dbg_->getZ80()->setHL1(value); break;
    case CZ80_REG_IX : dbg_->getZ80()->setIX (value); break;
    case CZ80_REG_I  : dbg_->getZ80()->setI  (value); break;
    case CZ80_REG_IY : dbg_->getZ80()->setIY (value); break;
    case CZ80_REG_R  : dbg_->getZ80()->setR  (value); break;
    case CZ80_REG_SP : dbg_->getZ80()->setSP (value); break;
    case CZ80_REG_PC : dbg_->getZ80()->setPC (value); break;
    case CZ80_REG_IFF: dbg_->getZ80()->setIFF(value); break;
    default          : assert(false);
  }

  dbg_->regChanged(reg_);
}
