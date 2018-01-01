Concept Application Server
==========================

A scalable webserver, a fast with small footprint programming language, a framework, a masterless multi-server document oriented with a SQL subset database engine (TinDB). It runs on FreeBSD, Linux, Windows and OS X, tested on x86, amd64 and ARM.

## The programing language

Concept is a simple javascript-like language, with a small difference: it is strict object-oriented. Also, all the code is compiled. Additionally it has a JIT compiler based on sljit. It is designed to be friendly and easy to user.

A simple console hello world app:
```
#!/usr/local/bin/concept

class Main {
  Main() {
    echo "Hello world!";
  }
}
```
The `function` keyword is optional.

The simplest form-based application it looks like this:
```
include Application.con
include RForm.con
include RLabel.con

class HelloForm extends RForm {
  private var labelHello;

  HelloForm(Parent) {
    super(Parent);
    labelHello = new RLabel(this);
    labelHello.Caption = "Hello world!";
    labelHello.Show();
  }
}

class Main {
  Main() {
    try {
      var Application = new CApplication(new HelloForm(null));
      Application.Init();
      Application.Run();
      Application.Done();
    } catch (var Exception) {
      echo "Didn’t catch $Exception";
    }
  }
}
```

The simplest async cgi application looks like this:
```
include async.con

class app {
    app(req) {
      req.header("Content-Type", "text/html; charset=utf-8");
      // req.usesession();
      // req.session.data["x"] = "test";

      // Using mustache templates:
      // req.template = "tpl/template.html";
      // req.put([
      //   "title" => "Hello world!",
      //   "content" => "Hello !"
      // ]);
      // or, no templates:
      req << "Hello world!";
  }
}
```

## The webserver

A full HTTP2 asynchronous webserver, written in Concept with an websocket application model.

## The framework

A rich framework, covering lots of protocols, controls, functions and classes.

## TinDB

TinDB is an asynchronous database engine written entirely in Concept. It features:
* Document-based objects stored in collections
* Indexes, including fulltext based on BM25 built-in support or Apache Lucy
* Stored procedures written in JavaScript(spidermonkey or duktape) or Concept
* Triggers
* Masterless replication in a ring configuration

A great thing about TinDB objects is that circular references are safe.

## Javascript Client (ConceptClient.js)

A webclient for websocket-driven extremely responsive, single-page web applications.

## Native C modules

Lots of wrappers for various library (over 7000 functions) starting from libxml2 and libcurl to Sphinx and WebKit.
