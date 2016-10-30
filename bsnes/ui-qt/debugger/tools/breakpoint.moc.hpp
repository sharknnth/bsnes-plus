class BreakpointItem : public QWidget {
  Q_OBJECT

enum {
  BreakAddrStart,
  BreakAddrDash,
  BreakAddrEnd,
  BreakData,
  BreakRead,
  BreakWrite,
  BreakExecute,
  BreakSource
};

public:
  QGridLayout *layout;
  QLineEdit *addr;
  QLineEdit *addr_end;
  QLineEdit *data;
  QCheckBox *mode_r;
  QCheckBox *mode_w;
  QCheckBox *mode_x;
  QComboBox *source;
  BreakpointItem(unsigned id);

  void setBreakpoint(string addr, string mode, string source);

public slots:
  void init();
  void toggle();

private:
  const unsigned id;
};

class BreakpointEditor : public Window {
  Q_OBJECT

public:
  QVBoxLayout *layout;
  BreakpointItem *breakpoint[SNES::Debugger::Breakpoints];
  QCheckBox *breakOnWDM;

  BreakpointEditor();

  void addBreakpoint(const string& addr, const string& mode, const string& source);

public slots:
  void toggle();
};

extern BreakpointEditor *breakpointEditor;
