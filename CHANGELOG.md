# Version 1.2
[X] More standard option implementations
  * #69, #70 NEW_ENVIRON
  
[x] MUD-specific option implementations
  * #67, #68 MCCP
  * #82, #83 MSDP
  
[x] Usability improvements
  * #79 Migrated from CppUnit to GTest testing framework
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
