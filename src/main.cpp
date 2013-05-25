#include <CQZ80Dbg.h>
#include <CZ80.h>
#include <QApplication>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  CZ80 z80;

  if (argc > 1)
    z80.load(argv[1]);

  CQZ80Dbg dbg(&z80);

  dbg.show();

  return app.exec();
}
