# Version 1.2
[X] More standard option implementations
  * NEW_ENVIRON
  
[x] MUD-specific option implementations
  * MCCP
  * MSDP
  
[x] Usability improvements
  * #110 Added byte_converter to filter out non-bytestream tokens
  
[x] Bug fixes
  * #95 Symbols are no longer hidden in static libraries
  * #112 Link dependencies spelled out properly
  
# Version 1.0 - Initial Offering
[x] Basic Telnet implementation
  * Command handling
  * Negotiation handling
  * Subnegotiation handling
  
[x] Abstractions for day-to-day Telnet management
  * client_option
  * server_option
  * session
  
[x] Reference standard option implementations
  * ECHO, SUPPRESS_GA and other "subnegotiationless" options
  * NAWS
