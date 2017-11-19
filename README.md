## About

Anope is an open source set of IRC Services. It is highly modular, with a vast number of configurable parameters, and is the most used IRC services package. There are also many modules on the [modsite](https://modules.anope.org) to add additional features. It runs on Linux, BSD, and Windows, and supports many modern IRCds, including InspIRCd, UnrealIRCd, and ircd-hybrid. For more details, credits, command line options, and contact information see [docs/README](https://github.com/anope/anope/blob/2.0/docs/README).

* [Website](https://anope.org)
* [GitHub](https://github.com/anope)
* IRC \#anope on irc.anope.org

## Installation

### Linux/BSD
Download the latest release off of the [releases page](https://github.com/anope/anope/releases).


```
$ ./Config
$ cd build
$ make
$ make install
```

Now change to the directory where you installed Anope to, e.g. `$ cd ~/services/`

### Windows
Download the latest release off of the [releases page](https://github.com/anope/anope/releases) and run the installer.


## Configuration

Copy conf/example.conf to conf/services.conf

```
$ cp conf/example.conf conf/services.conf
```

Edit services.conf, configuring the uplink, serverinfo, and protocol module configurations. Example link blocks for popular IRCds are included in the the example.conf documentation. The [Anope wiki](https://wiki.anope.org) is also a good source of information. Our support channel is located at #anope on [irc.anope.org](irc://irc.anope.org/#anope).

Note that the example configuration file includes other example configuration files. If you want to modify the other example configuration files, copy them (e.g. `modules.example.conf` to `modules.conf`) and modify the `include` directive in `services.conf` to include the new file.

## Running

Run `$ ./bin/services` to start Anope. If asked to provide logs for support, use the `--support` flag, e.g.: `$ ./bin/services --support`

## Installing extra modules

Extra modules, which are usually modules which require extra libraries to use, such as m\_mysql, can be enabled with the `./extras` command from the source directory. Then re-run `Config`, `make` and `make install` again. Third party modules can be installed by placing them into the `modules/third` directory.
