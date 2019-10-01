# Contributing to FSeam

**Contents**<br>
[Branches](#branches)<br>
[Testing your changes](#testing-your-changes)<br>
[Documenting your code](#documenting-your-code)<br>
[Code constructs to watch out for](#code-constructs-to-watch-out-for)<br>

So you want to contribute something to FSeam? That's great! Whether it's a bug fix, a new feature, support for
additional compilers - or just a fix to the documentation - all contributions are very welcome and very much appreciated.
Of course so are bug reports and other comments and questions.

If you are contributing to the code base there are a few simple guidelines to keep in mind. This also includes notes to
help you find your way around. As this is liable to drift out of date please raise an issue or, better still, a pull
request for this file, if you notice that.

## Branches

Ongoing development is currently on _master_. At some point an integration branch will be set-up and PRs should target
 that - but for now it's all against master. You may see feature branches come and go from time to time, too.  
If you plan to integrate a big new feature, you can ask via issue to create such feature branch in order to target this with
your PR.


## Testing your changes

Obviously all changes to FSeam's code should be tested. If you added new
functionality, you should add tests covering and showcasing it. Even if you have
only made changes to FSeam internals (i.e. you implemented some performance
improvements), you should still test your changes.

## Documenting your code

If you have added new feature to FSeam, it needs documentation, so that
other people can use it as well. This section collects some technical
information that you will need for updating FSeam's documentation, and
possibly some generic advise as well.

* In case of new features : You need to create the appropriate paragraphe in
  the Markdown documentation. 
* Documentation in code is mandatory for public api call (call directly used
  by FSeam's client) in order to explain the purpose of the newly created API call.
  The documentation in code has to be written in the header files as a Doxygen comments.

----

_This documentation will always be in-progress as new information comes
up, but we are trying to keep it as up to date as possible._

---

[Home](Readme.md#top)
