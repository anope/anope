# Anope REST API

Anope provides a RESTful HTTP API through the rest and httpd modules which
expose basic CRUD operations on all of Anope's internal data structures.

The type names and data structures are the same as is used in the SQL/RDBMS
databaes support, but it otherwise currently not documented.

The RESTful API documented here is relatively low level and allows you to
construct things that aren't normally possible in Anope, such as creating
nickless accounts or having an accounts display not matched a grouped nicks
name. This would produce weird behavior and not work as expected. It is
your responsibility to not do this.

As an alternative you may execute commands remotely using the xmlrpc_main
module, which uses Anope's normal command system and is less subject to misuse.

# Example usage

## Getting available object types

```
$ curl localhost:8080/rest/types
{
    "types": [
        "botinfo",
        "xline",
        "oper",
        "badword",
        "channel",
        "level",
        "mlockmode",
        "accesschanaccess",
        "akick",
        "entrymsg",
        "flagschanaccess",
        "logsetting",
        "modelock",
        "csmiscdata",
        "cssuspendinfo",
        "xopchanaccess",
        "vhost",
        "hostrequest",
        "memoinfo",
        "memo",
        "memoignore",
        "nick",
        "account",
        "mode",
        "nsaccess",
        "autojoin",
        "nsmiscdata",
        "nssuspendinfo",
        "forbid",
        "ignore",
        "operinfo",
        "newsitem",
        "exception",
        "stats"
    ]
}
```

The types are subject to change, as several are dynamically added via modules,
including third party modules.

## Listing objects of a type

Send a GET to /rest/{object type} to return a list of all objects of that type.
For example, to fetch a list of all accounts.

```
$ curl localhost:8080/rest/account
{
    "account": [
        {
            "display": "Adam",
            "pass": "sha256:55ad6396a287df38967de578cbf92788faea8c94cd7d0e4f55eb45257921d75b:3df6978927389fb71a560a232572e5871bd072752eca27f159dc1fdc2462411c",
            "email": "adam@sigterm.info",
            "id": "10"
        },
        {
            "display": "Adam2",
            "pass": "sha256:4cbb6ec03a8e7401342ee8832f5e4bd2abeb8fc7a007e66e2ac02e0c14f694f0:041167d04d70381e4218781e06b2b57b08165a987aced63f0a8ca8d25ac8ee51",
            "email": "adam@anope.org",
            "id": "95"
        }
    ]
}
```

## Searching for objects

You may search for objects by the value of one of their fields, if it is unique.
This is most useful for looking up nicks, accounts, and channels.

If the field you are searching for is not something commonly looked up by Anope,
which is mostly anything other than nick, account, and channel names, it will
likely cause the SQL database backend to on-the-fly index the field.

Send a GET request to /rest/{object type}/{field name}/{search}

```
$ curl localhost:8080/rest/account/display/Adam
{
    "display": "Adam",
    "pass": "sha256:55ad6396a287df38967de578cbf92788faea8c94cd7d0e4f55eb45257921d75b:3df6978927389fb71a560a232572e5871bd072752eca27f159dc1fdc2462411c",
    "email": "adam@sigterm.info",
    "id": "10"
}
```

Which will return the entire object. If it is not found, a non-200 status code is returned,
along with an error message. Error messages are never JSON formatted, but are simple strings.

## Getting objects

If you have an object ID, you can also fetch the object by id.

Send a GET request to /rest/{object type}/{id}

```
$ curl localhost:8080/rest/account/10
{
    "display": "Adam",
    "pass": "sha256:55ad6396a287df38967de578cbf92788faea8c94cd7d0e4f55eb45257921d75b:3df6978927389fb71a560a232572e5871bd072752eca27f159dc1fdc2462411c",
    "email": "adam@sigterm.info",
    "id": "10"
}
```

## Updating the value of a field of an existing object

You may PUT to /rest/{object type}/{id}/{field} data to update the value of a field.

```
$ curl -D - -X PUT localhost:8080/rest/account/10/email -d 'adam@anope.org'
HTTP/1.1 200 OK
Date: Sun, 30 Oct 2016 17:17:05 EDT
Server: Anope-2.1
Content-Type: application/json
Content-Length: 0
Connection: Close

```

Again, a 200 status code is okay, anything else is an error, and a description is provided in the reply content.
You can now fetch the object back and notice the changed email address.

## Creating objects

You can create new objects by POSTing to /rest/{object type}

```
$ curl -D - -X POST localhost:8080/rest/account -d '{"display": "Adam3", "email": "Adam@anope.org"}'
HTTP/1.1 201 Created
Date: Sun, 30 Oct 2016 17:39:56 EDT
Server: Anope-2.1
Content-Type: application/json
Content-Length: 3
Connection: Close

126
```

This will return a 201 Ceated, with the body being the object's new id.

## Deleting objects

Similarly you can delete objects by sending DELETE to /rest/{object type}/{id}.

```
$ curl -D - -X DELETE localhost:8080/rest/account/126
HTTP/1.1 200 OK
Date: Sun, 30 Oct 2016 17:42:40 EDT
Server: Anope-2.1
Content-Type: application/json
Content-Length: 0
Connection: Close
```

Due to the way Anope stores its data, deleting any object at any time should be safe, and will not leak
other (perhaps now unreachable) objects.

# Object References and Relationships

Object references are represented in the REST API by {type}:{id}

```
$ curl localhost:8080/rest/nsaccess/13
{
    "account": "account:10",
    "mask": "Adam@*.gvd.168.192.IP",
    "id": "13"
}
```

Such as the value of "account" here for this nickserv access entry. Even though object ids within Anope
are global and not specific to the type. As such, if you submit a POST or PUT operation which creates a
field that is a reference, it is expected you supply both the type and id, in this format.

It is possible to search for object references of a specific type to a given object. This is how all
one-to-many relationships are looked up within Anope. Objects never contain lists of other objects.
There are no join tables, and the objects within the C++ API do not contain collections of other
objects.

For example:

```
$ curl localhost:8080/rest/account/10?refs=nsaccess
{
    "account": [
        {
            "account": "account:10",
            "mask": "Adam@*.gvd.168.192.IP",
            "id": "13"
        },
        {
            "account": "account:10",
            "mask": "adam@anope.org",
            "id": "126"
        }
    ]
}
```

This fetches all objects of type nsaccess which have a reference to an account with id 10. Because
the type nsaccess only has one field that is a reference, the result is effectively the account's
NickServ access list.

Because searching by refs searches object references and not at a field level, in cases of searching
for objects with mutltiple references, you would have to inspect the resulting objects to see if it
is what you want. For example, if you wanted to find all channels owned by a user, you may perform
something like GET /rest/account/10?refs=channel. The channels returned have a reference to the
given account, whether that is the channels founder or something else, like successor, or any other
field possible (that may have even been added by a 3rd party module), is unknown.

Relationships defined within Anope, that is, the fields on objects which are object references, can
optionally depend on the existance of the object to which they reference. These fields can be seen
within the SQL schema as having foreign keys with ON CASCADE DELETE. When an object is deleted, all
references to it automatically get either NULLed, or the objects which reference it get recursively
deleted too. This is why it is generally safe to delete any object at any time.

The field "account" on "nsaccess" is one that will cascade delete, which is why if you delete an
account, the access entries for it are automatically also cleaned up.
