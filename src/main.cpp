#include <CQZ80Dbg.h>
#include <CZ80.h>
#include <QApplication>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  std::string filename;
  bool        binary = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-bin") {
      ++i;

      if (i < argc) {
        filename = argv[i];
        binary   = true;
      }
    }
    else {
      filename = argv[i];
    }
  }

  CZ80 z80;

  if (filename != "") {
    if (binary)
      z80.loadBin(filename);
    else
      z80.load(filename);
  }

  CQZ80Dbg dbg(&z80);

  dbg.show();

  return app.exec();
}
