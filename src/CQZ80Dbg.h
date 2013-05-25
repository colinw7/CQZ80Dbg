#ifndef CQZ80_DBG_H
#define CQZ80_DBG_H

#include <QWidget>
#include <CZ80.h>

#include <string>
#include <set>

class CZ80;
class QTextEdit;
class QCheckBox;
class QLineEdit;
class QScrollBar;
class CQZ80Dbg;

struct CQZ80MemLine {
  CQZ80MemLine(const std::string &pcStr_="", const std::string &memStr_="",
               const std::string textStr_="") :
   pcStr(pcStr_), memStr(memStr_), textStr(textStr_) {
  }

  std::string pcStr;
  std::string memStr;
  std::string textStr;
};

class CQZ80Mem : public QWidget {
  Q_OBJECT

 public:
  CQZ80Mem(CQZ80Dbg *dbg);

  void setLine(uint i, const std::string &pcStr, const std::string &memStr,
               const std::string &textStr);

 public slots:
  void sliderSlot(int y);

 private:
  void paintEvent(QPaintEvent *);

  void mouseDoubleClickEvent(QMouseEvent *e);

 private:
  typedef std::vector<CQZ80MemLine> LineList;

  CQZ80Dbg* dbg_;
  LineList  lines_;
  int       y_offset_;
  int       char_width_;
  int       char_height_;
  int       dx_;
};

//------

struct CQZ80InstLine {
  CQZ80InstLine(uint pc_=0, const std::string &pcStr_="", const std::string &codeStr_="",
                const std::string textStr_="") :
   pc(pc_), pcStr(pcStr_), codeStr(codeStr_), textStr(textStr_) {
  }

  uint        pc;
  std::string pcStr;
  std::string codeStr;
  std::string textStr;
};

class CQZ80Inst : public QWidget {
  Q_OBJECT

 public:
  CQZ80Inst(CQZ80Dbg *dbg);

  void clear();

  void setLine(uint pc, const std::string &pcStr, const std::string &codeStr,
               const std::string &textStr);

  uint getNumLines() const { return line_num_; }

  bool getLineForPC(uint pc, uint &line_num) const;
  uint getPCForLine(uint line_num);

  void clearBreakpoints() { breakpoints_.clear(); }

  void addBreakPoint(uint pc) { breakpoints_.insert(pc); }

  void mouseDoubleClickEvent(QMouseEvent *e);

 public slots:
  void sliderSlot(int y);

 private:
  void paintEvent(QPaintEvent *);

 private:
  typedef std::vector<CQZ80InstLine> LineList;
  typedef std::map<uint,uint>        PCLineMap;
  typedef std::set<uint>             BreakpointList;

  CQZ80Dbg*      dbg_;
  LineList       lines_;
  int            y_offset_;
  int            char_height_;
  int            line_num_;
  PCLineMap      pc_line_map_;
  PCLineMap      line_pc_map_;
  BreakpointList breakpoints_;
};

//------

class CQZ80RegEdit : public QWidget {
  Q_OBJECT

 public:
  CQZ80RegEdit(CQZ80Dbg *dbg, CZ80Reg reg);

  void setValue(uint value);
  uint getValue() const;

 private:
  CQZ80Dbg  *dbg_;
  CZ80Reg    reg_;
  QLineEdit *edit_;

 private slots:
  void valueChangedSlot();
};

//------

class CQZ80Dbg : public QWidget, public CZ80Trace {
  Q_OBJECT

 public:
  CQZ80Dbg(CZ80 *z80);

  CZ80 *getZ80() const { return z80_; }

  void regChanged(CZ80Reg reg);

  const QFont &getFixedFont() const { return fixed_font_; }

 private:
  void addWidgets();

  void setMemoryText();
  void setMemoryLine(uint pos);

  std::string getByteChar(uchar c);

  void setInstructionsText();

  void setStackText();

  void setBreakpointText();

  QLineEdit *createRegisterEdit();

  //----

 private:
  void postStepProc();

  void memChanged(ushort pos, ushort len);

  void breakpointsChanged();

 private slots:
  void addBreakpointSlot();
  void deleteBreakpointSlot();
  void clearBreakpointSlot();

  void setTraceSlot(int trace);

  void runSlot();
  void nextSlot();
  void stepSlot();
  void continueSlot();
  void stopSlot();
  void restartSlot();
  void exitSlot();

 private:
  CZ80  *z80_;
  QFont  fixed_font_;

  bool follow_;

  CQZ80Mem   *memory_text_;
  QScrollBar *memory_vbar_;
  CQZ80Inst  *instructions_text_;
  QScrollBar *instructions_vbar_;
  QLineEdit  *op_data_;
  QTextEdit  *stack_text_;
  QTextEdit  *breakpoints_text_;

  QCheckBox *s_flag_checkbox_;
  QCheckBox *z_flag_checkbox_;
  QCheckBox *y_flag_checkbox_;
  QCheckBox *h_flag_checkbox_;
  QCheckBox *x_flag_checkbox_;
  QCheckBox *p_flag_checkbox_;
  QCheckBox *n_flag_checkbox_;
  QCheckBox *c_flag_checkbox_;

  CQZ80RegEdit *af_edit_;
  CQZ80RegEdit *af1_edit_;
  CQZ80RegEdit *bc_edit_;
  CQZ80RegEdit *bc1_edit_;
  CQZ80RegEdit *de_edit_;
  CQZ80RegEdit *de1_edit_;
  CQZ80RegEdit *hl_edit_;
  CQZ80RegEdit *hl1_edit_;
  CQZ80RegEdit *ix_edit_;
  CQZ80RegEdit *i_edit_;
  CQZ80RegEdit *iy_edit_;
  CQZ80RegEdit *r_edit_;
  CQZ80RegEdit *sp_edit_;
  CQZ80RegEdit *iff_edit_;
  CQZ80RegEdit *pc_edit_;
};

#endif
