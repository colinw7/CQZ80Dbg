#ifndef CQZ80Instructions_H
#define CQZ80Instructions_H

#include <QFrame>
#include <set>

class CQZ80Dbg;
class QScrollBar;

class CQZ80InstLine {
 public:
  CQZ80InstLine(uint pc=0, const std::string &pcStr="", const std::string &codeStr="",
                const std::string textStr="") :
   pc_(pc), pcStr_(pcStr), codeStr_(codeStr), textStr_(textStr) {
  }

  ushort pc() const { return pc_; }

  uint num() const { return pc_ / 8; }

  const std::string &pcStr  () const { return pcStr_  ; }
  const std::string &codeStr() const { return codeStr_; }
  const std::string &textStr() const { return textStr_; }

 private:
  uint        pc_ { 0 };
  std::string pcStr_;
  std::string codeStr_;
  std::string textStr_;
};

//------

class CQZ80Inst : public QFrame {
  Q_OBJECT

 public:
  CQZ80Inst(CQZ80Dbg *dbg);

  void setVBar(QScrollBar *vbar) { vbar_ = vbar; }

  void setFont(const QFont &font);

  void clear();

  void setLine(uint pc, const std::string &pcStr, const std::string &codeStr,
               const std::string &textStr);

  uint getNumLines() const { return lineNum_; }

  bool getLineForPC(uint pc, uint &lineNum) const;
  uint getPCForLine(uint lineNum);

  void clearBreakpoints() { breakpoints_.clear(); }

  void addBreakPoint(uint pc) { breakpoints_.insert(pc); }

  void mouseDoubleClickEvent(QMouseEvent *e);

  void contextMenuEvent(QContextMenuEvent *event);

  void reload();

 public slots:
  void sliderSlot(int y);

  void dumpSlot();

  void reloadSlot();

 private:
  void paintEvent(QPaintEvent *);

 private:
  typedef std::vector<CQZ80InstLine> LineList;
  typedef std::map<uint,uint>        PCLineMap;
  typedef std::set<uint>             BreakpointList;

  CQZ80Dbg*      dbg_        { nullptr };
  QScrollBar*    vbar_       { nullptr };
  LineList       lines_;
  int            yOffset_    { 0 };
  int            charHeight_ { 8 };
  int            lineNum_    { 0 };
  PCLineMap      pcLineMap_;
  PCLineMap      linePcMap_;
  BreakpointList breakpoints_;
};

#endif
