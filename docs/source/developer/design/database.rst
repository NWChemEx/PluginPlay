.. _database_design:

###############
Database Design
###############

This section explores the design considerations that went into selecting a
database backend for the ``Cache`` class.

*******************
What is a Database?
*******************

The usual definition is something along the lines of: a database is an organized
collection of data stored and accessed electronically. What really sets a
database apart from say a ``std::map`` or a spreadsheet is that databases are
designed to contain a lot of data and to be accessed concurrently by a
potentially large number of users. The secret behind database performance is
that users do not directly interact with the a database, but rather they
interact with a database management system (DBMS). The DBMS is responsible for
managing access and memory based on user requests.

From the perspective of PluginPlay, the imporant thing to note is that most
modern databases are key-value containers, designed to hold copious amounts of
data, and your typical DBMS provides loads of features related to data
retention, processing, and access.

Database Terminology
====================

The reader new to databases will quickly be inundated with jargon. This
subsection provides a small glossery of the most common terms.

ACID
  Stands for atomicity, consistency, isolation, and durability. Each transaction
  on a database must minimally satisfy these four properties for the database to
  be considered reliable.

CRUD
  Stands for create, read, update, and delete. These are the four foundational
  operations of any database.

DBMS
  Database management system. The library providing the API for interacting with
  a database.

MVCC
  Stands for multiversion concurrency control. MVCC avoids locks by storing
  multiple copies of the data. When data is updated a new version is added
  alongside older versions which may still be in use.

ODBC
  Open database connectivity. A standardized API for expressing database
  operations. The API is DBMS independent. Library developers can write their
  code using the ODBC API and it will work with whatever DBMS, the ODBC driver
  manager supports.

ORM
  Stands for object-relational mapping. Many common databases only support
  primitive data types like integers, floats, and strings. ORM is a technique
  where the objects are maintained in a virtual database and their members are
  mapped to enteries in the real database.

RDBMS
  Relational DBMS. A DBMS that specifically deals with relational databases.
  Relational databases are typically thought of as being tables with columns and
  rows.

SQL
  Structured query language. The de facto standard language for interacting with
  RDBMS.

WAL
  Stands for write-ahead logging. Techniques for the AC of ACID. Changes are
  first recorded in a log. The log is written to stable storage before being
  applied to the database. In the event of a restart the database can be
  compared to the log to figure out what operations need to be done still.

***********************
Database Considerations
***********************

The database backend is the lowest level of the ``Cache`` and is responsible for
addressing all memoization and checkpoint/restart (C/R) design considerations
which were not handled by the ``Cache``. Generally speaking they fall into the
following categories:

Parallel Nature of PluginPlay
=============================

- Avoid data races
- Will need to store parallel objects, sometimes in their distributed form and
  sometimes in a serialized form
- May want to use parallel filesystems for the checkpoint
- May want multiple checkpoint files
- Parallel systems may have more complicated memory hierarchies and we want to
  use them effectively.
- Resiliency to deal with hardware failures

Object Storage
==============

- Will need to serialize the objects
- Want a quick access (not serialized) and a long-term serialized form
- May not want to serialize all objects at the same frequency
- Compression (presumably of the serialized objects)

Other
=====

- Access scenarios
- Automatic backups
- Diff support

  - Checkpoint the original state and the changes to the original state needed
    to make the current state

************************
Database Implementations
************************

Databases are some of the most important software elements and are used in a ton
of applications. Thus it should come as no surprise that there are a lot of C++
options for databases. We have grouped the options based on whether they are
stand alone, or if they extend an existing database. This section is meant to be
a list of options with some notes, rather than an extensive comparison. The
next section provides a head-to-head comparison of the most promising options.

Stand Alone Options
===================

- ClickHouse

  - https://github.com/ClickHouse/clickhouse-cpp
  - C++ client library for ClickHouse
  - column-oriened DBMS
  - data compression
  - disk storage
  - C++ API seems poorly documented and to rely on string parsing
  - Limited type support
  - Apache 2.0
  - Appears to be actively maintained. 143 stars and 22 watchers.

- Sophia

  - https://github.com/pmwkaa/sophia
  - MVCC key-value database
  - RAM-disk hybrid
  - Compression
  - BSD
  - Likely abandoned (last commit January 2019). 1.8K stars and 106 watchers.

- TileDB

  - https://github.com/TileDB-Inc/TileDB
  - Support for dense and sparse arrays, dataframes, key-value stores
  - Parallel I/O
  - Compression
  - Data versioning
  - Threaded
  - I/O and RAM versions
  - MIT License
  - Actively maintained. 1.3K stars and 61 watchers.

- Bedrock

  - https://github.com/Expensify/Bedrock
  - Networking and distributed transaction layer built atop SQLite
  - Does not appear to be a SQLite client
  - LGPL-3.0 License
  - Active development. 801 stars and 70 watchers.

- upscaledb

  - https://upscaledb.com/
  - Key-value based
  - Supports compression
  - can run in-memory database
  - Apache 2.0
  - Last commit Aug 2021. 521 stars and 40 watchers.

- simdb

  - https://github.com/LiveAsynchronousVisualizedArchitecture/simdb
  - single-file library
  - hash based key-value
  - Apache 2.0
  - Likely abandoned (last commit April 2018). 425 stars and 42 watchers.

- ObjectBox

  - https://objectbox.io/
  - Designed for embedded devices, internet of things, and mobile
  - Looks very invasive (have to wrap data members so ObjectBox knows about
    them)
  - Apache 2.0
  - Active development. 119 stars and 12 watchers.

- DTL

  - http://dtemplatelib.sourceforge.net/
  - Likely abandoned (last updated April 2013)

- EasyQTSql

  - https://github.com/kramolnic/EasyQtSql
  - Appears to be a wrapper around Qt's SQL library
  - Likely abandoned (last commit May 2019). 37 stars and 3 watchers.

LevelDB
=======

LevelDB is a Google project which is widely used. Facebook used LevelDB for the
guts of its (also widely used) RockDB project.

- LevelDB
  - https://github.com/google/leveldb
  - key/value arbitrary byte data
  - automatic compression
  - support for threaded access
  - can get snapshots (immutable representations)
  - Disk based (using mulitple files)
  - Checksums for data integrity
  - Appears to be actively maintained. 28.5K stars and 1.3K watchers.

- RocksDB

  - https://github.com/facebook/rocksdb
  - Built on LevelDB
  - Designed for storing data on flash drives
  - Can adapt to pure memory
  - Supports backups and checkpoints
  - GPLv2 or Apache 2.0
  - Appears to be actively maintained. 22K stars and 1K watchers.

- BerylDB

  - https://github.com/beryldb/beryldb
  - Built on RocksDB
  - Supports storing lists, sets, multimaps, and keys
  - BSD
  - Last commit November 2021. 197 stars and 6 watchers.

LMDB
====

- Lightning Memory-Mapped Database

  - https://www.symas.com/lmdb
  - Developed as part of the `OpenLDAP project <https://www.openldap.org/>`
  - key-value based
  - Webpages is a bit of a maze
  - OpenLDAP Public License
  - GitHub mirror hasn't been updated since Jun 2021. 1.9K stars and
    158 watchers.

- libmdbx

  - https://github.com/erthink/libmdbx
  - Descendant of LMDB
  - Appears to be in a transition period to MithrilDB
  - OpenLDAP Public License
  - Active development. 1K stars and 48 watchers.

- lmdbxx

  - https://github.com/drycpp/lmdbxx
  - C++ wrapper for LMDB
  - Single header
  - Unlicense license
  - Likely abandoned (last commit Feb 2016). 247 stars and 17 watchers.

MySQL
=====

- MySQL++

  - https://github.com/tangentsoft/mysqlpp

  - LGPLv2
  - Last commit Dec 2021. 44 stars and 4 watchers.

    - GitHub repo is an official mirror (so statistics may not be accurate)


MongoDB
=======

- mongo-cxx-driver

  - https://github.com/mongodb/mongo-cxx-driver
  - Apache 2.0
  - Actively maintained. 869 stars and 131 watchers.

- mongo-c-driver

  - https://github.com/mongodb/mongo-c-driver
  - Same organization as the C++ driver
  - Likely has a similar feature set to the C++ driver
  - Apache 2.0
  - Actively maintained. 716 stars and 81 watchers.

PostgreSQL
==========

- libpqxx

  - http://pqxx.org/development/libpqxx/
  - Official C++ API for PostgreSQL
  - BSD
  - Active development. 606 stars and 36 watchers.

- pgfe

  - https://github.com/dmitigr/pgfe/
  - Zlib License
  - Active development. 111 stars and 10 watchers.

- taoPQ

  - https://github.com/taocpp/taopq

  - Boost Software License
  - Active development. 191 stars and 17 watchers.

Redis
=====

These are C/C++ interfaces to a Redis database.

- Hiredis

  - https://github.com/redis/hiredis
  - C client library for interacting with a Redis database
  - BSD
  - Actively maintained. 5.2K stars and 303 watchers.

- cpp_redis

  - https://github.com/cpp-redis/cpp_redis
  - C++11 Asynchronous multi-platform lightweight Redis client
  - MIT license
  - Last commit Aug 2021. 478 stars and 29 watchers.

- redis-cpp

  - https://github.com/tdv/redis-cpp/
  - Based on hiredis
  - MIT License
  - Last commit Nov 2021. 61 stars and 10 watchers.

- redis-plus-plus

  - https://github.com/sewenew/redis-plus-plus
  - Apache 2.0
  - Active development. 743 stars and 25 watchers.

- redis3m

  - https://github.com/luca3m/redis3m
  - Wraps hiredis in a C++ API
  - Apache 2.0 License
  - Likely abandoned (last commit May 2019). 182 stars and 30 watchers.

- cpp-bredis

  - https://github.com/basiliscos/cpp-bredis
  - header-only
  - synchronous and asynchronous APIs
  - MIT
  - Last commit December 2021. 140 stars and 11 watchers.

- aredis

  - https://github.com/lordoffox/aredis
  - No documentation, just a short code snippet.
  - Boost Software License
  - Likely abandoned (last commit May 2018). 26 stars and 5 watchers

RethinkDB
=========

- librethinkdbxx

  - https://github.com/AtnNn/librethinkdbxx
  - According to README it's still in early stages of development.
  - Apache 2
  - Likely abandoned (last commit Nov 2017). 91 stars and 17 watchers.

SQLite
======

- SQLiteC++

  - https://github.com/SRombauts/SQLiteCpp
  - Actively maintained. 1.4K stars and 75 watchers.

- SQLite ORM

  - https://github.com/fnc12/sqlite_orm
  - Supports custom types
  - STL compatible
  - Supports memory and disk databases
  - GNU Affero General Public License
  - Last commit December 2021. 1.5K stars and 61 watchers.

- sqlite modern cpp wrapper

  - MIT License
  - Doesn't seem to support custom types
  - Likely abandoned (last commit November 2020). 671 stars and 58 watchers.

- hiberlite

  - ORM for SQLite relying on a Boost Serialization-like API
  - Aside from the serialization-like API not sure what other features it has
  - BSD
  - Likely abandoned (last commit June 2018). 614 stars and 58 watchers.

- VSQLite++

  - https://github.com/vinzenz/vsqlite--
  - C++ wrapper for SQLite3
  - Little documentation.
  - BSD
  - Likely abandoned (last commit Nov 2020). 27 stars and 5 watchers.

Multiple Databases
==================

The options in this category provide interfaces to more than one database type.

- nanodbc

  - https://github.com/nanodbc/nanodbc
  - Interacts with multiple DBMS through the ODBC C API.
  - MIT license
  - Active development. 228 stars and 17 watchers.

- ODB

  - https://www.codesynthesis.com/products/odb/
  - Interface to MySQL, SQLite, PostgreSQL, Oracle, and Microsoft SQL.
  - GPLv2
  - Actively maintained.

- OTL

  - http://otl.sourceforge.net/
  - C++ template library for interacting with Oracle, ODBC, DB2-CLI
  - Actively maintained.

- QTL

  - https://github.com/znone/qtl
  - Supports MySQL, SQLite, PostgreSQL, and ODBC
  - Apache 2.0
  - Last commit Oct 2021. 130 stars and 10 watchers.

- QxORM

  - https://www.qxorm.com/qxorm_en/home.html
  - SQLite, MySQL, PostgreSQL, Oracle, Microsoft SQL, MongoDB
  - GPLv3 or under the QxORM Propietary License (if you pay)

- SOCI

  - https://github.com/SOCI/soci
  - Common API for a number of backend databases including: DB2, Firebird,
    MySQL, ODBC, Oracle, PostgreSQL, and SQLite 3
  - Boost Software License
  - Actively maintaiend. 1.1K stars and 100 watchers.

- SQLAPI++

  - https://www.sqlapi.com/
  - Propietary interface to Oracle, SQL Server, DB2, Sybase, MySQL, MariaDB,
    PostgreSQL, SQLite, Informix, Interbase, Firebird, SQLBase, SQL Anywhere,
    and ODBC
  - SQLAPI++ Library License

- sqlpp11

  - https://github.com/rbock/sqlpp11
  - MariaDB, MySQL, PostgreSQL, SQLite3, and SQLCipher
  - BSD
  - Active development. 1.9K stars and 115 watchers.

- Tntdb

  - https://github.com/maekitalo/tntdb
  - Supports SQLite, MySQL, Postgresql, and Oracle
  - LGPL-2.1
  - Actively maintained. 30 stars and 13 watchers.

***********************************
Database Implementation Comparsions
***********************************

The goal of this section is to come to a decision on what database to use. To
this end. We have enumerated a set of features we are looking for.

#. Widely supported and stable

   - The database is going to be an integral part of PluginPlay. We don't want
     to adopt a solution which looses support in a few years or has to do a
     complete API rewrite

#. License compatible with Apache 2.0

#. Designed for concurrent access.

#. Allowances for object-oriented applications.

   - Should support storing objects in their native form when they are in
     memory and serializing/deserializing them to put them on disk.

#. On disk storage

   - It seems like a lot of commercial applications want to use IP
     addresses for specifying the location of the database.

   - AFAIK I/O to a parallel file system is done the same way as a normal file
     system, i.e. you open a file on it by specifying the path. The parallel FS
     just shows up as a different mount point.

#. ACID Compliant (i.e. resilient)

The previous section listed a lot of C++ database solutions. Our focus on
solutions which are widely supported narrows the field considerably. The
following table summarizes how the widely supported choices stack up against our
criteria.

.. |x| replace:: check
.. |o| replace:: -

+------------------+---------+---------+------------+-----+------+------+
| Database Name    | Support | License | Concurrent | OOP | Disk | ACID |
+ =================+=========+=========+============+=====+======+======+
| TileDB           |
+------------------+---------+---------+------------+-----+------+------+
| LevelDB          |
+------------------+---------+---------+------------+-----+------+------+
| RocksDB          |
+------------------+---------+---------+------------+-----+------+------+
| mongo-cxx-driver |
+------------------+---------+---------+------------+-----+------+------+
| libpqxx          |
+------------------+---------+---------+------------+-----+------+------+
| Hiredis          |
+------------------+---------+---------+------------+-----+------+------+
| redis-plus-plus  |
+------------------+---------+---------+------------+-----+------+------+
| SQLiteC++        |
+------------------+---------+---------+------------+-----+------+------+
| SQLite ORM       |
+------------------+---------+---------+------------+-----+------+------+
