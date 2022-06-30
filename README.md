# My overkill personal finance software

This is (going to be) my app for managing my personal finances and budgeting, pretty much mostly based on YNAB.

This is absolutely not production ready.

# Architecture
- Server/client architecture
- Backend is a Django based Python server with a REST API
- The frontend for now is a C++ terminal UI written with FTXUI, however I may implement other front ends

# Roadmap
- Emulation of YNAB-like features
  - Monthly envelope system budgeting
  - Transaction ledger
  - Reports
- Bank import?
- Stock tracking?
- Other UI frontends consuming the API

# Why?
I've done this to mostly keep in practice for various languages and skills which means that a lot of the design decisions don't actually make a lot of sense. For example, the frontend is written as a terminal interface in C++. There is not really any reason for this other than that I wanted to practice my C++ and play around with some of the newer C++20 features.


