#include <CQZ80Dbg.h>
#include <CZ80PortData.h>
#include <CZ80RstData.h>
#include <CQApp.h>
#include <CZ80.h>

class CPMPortData : public CZ80PortData {
 public:
  CPMPortData(CZ80 &z80) :
   CZ80PortData(z80) {
  }

  uchar in(uchar port, uchar qual) override {
    if (port == 0) {
      uchar c = z80.getC();

      if      (c == 2) {
        uchar e = z80.getE();

        std::cerr << char(e);
      }
      else if (c == 9) {
        ushort de = z80.getDE();

        for (int i = 0; ; ++i) {
          char c = z80.getByte(de + i);

          if (c == '$')
            break;

          std::cerr << c;
        }
      }
    }
    else
      std::cerr << std::hex << int(port) << " " << std::hex << int(qual) << std::endl;

    return 0;
  }
};

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  std::string filename;

  bool binary = false;
  bool test   = false;
  bool cpm    = false;
  int  org     = 0;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if      (arg == "-bin") {
      binary = true;
    }
    else if (arg == "-org") {
      ++i;

      if (i < argc) {
        org = atoi(argv[i]);
      }
    }
    else if (arg == "-test") {
      test = true;
    }
    else if (arg == "-cpm") {
      cpm    = true;
      org    = 0x100;
      binary = true;
    }
    else {
      filename = argv[i];
    }
  }

  //---

  CZ80 z80;

  z80.setLoadPos(org);

  if (filename != "") {
    if (binary)
      z80.loadBin(filename);
    else
      z80.load(filename);
  }

  if (cpm) {
    z80.setByte(0, 0xd3);
    z80.setByte(1, 0x00);

    z80.setByte(5, 0xdb);
    z80.setByte(6, 0x00);
    z80.setByte(7, 0xc9);

    z80.setAllowInterrupts(false);
  }

  CZ80StdRstData rst_data(z80);

  if (test)
    z80.setRstData(&rst_data);

  CPMPortData cpm_data(z80);

  if (cpm)
    z80.setPortData(&cpm_data);

  CQZ80Dbg dbg(&z80);

  dbg.init();

  dbg.setFixedFont(QFont("Courier New", 16));

  dbg.show();

  return app.exec();
}
