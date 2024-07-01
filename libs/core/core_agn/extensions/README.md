Module contains "protocol extensions" that are split parts of the protocol:
- [logic](./logic/) - manages protocol's starting and restarting procedures,
- [setter](./setter/) - manages setting local serial port and remote at startup; also is able to change settings while communication has already started,
- [exchanger](./exchanger/) - controls data exchange between local serial and remote (including packet numbering, etc.).

Protocol extensions are implementation-agnostic and can be easily tested.
Low-level implementation can be easily changed by specifying different classes to extensions (which are class templates) with appropriate API used by extensions.
Interfaces used can be found in [sides directory](./../sides/).

Other than above, one can find:
- [base](./base/) contains base class for extensions to derive from,
- [common](./commons/) contains definitions used by many extensions,
- [ext_except](./ext_except/) - exceptions that modules can send when an exception run into some error.