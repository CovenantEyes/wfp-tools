WFP Tools
=========

Tools to help understand Windows Filtering Platform craziness.


Dependencies
============

These projects build with **Microsoft Visual Studio 2010** and use Microsoft **Windows SDK 7.1**.


Building with Microsoft Windows SDK
-----------------------------------

Both Microsoft's Windows SDK 7.0A and 7.1 (and perhaps other versions too) contain bugs in the following files:

  * `include\fwptypes.h`
  * `include\iketypes.h`

In several places, these headers have extraneous new-line characters in the middle of `#define` derectives. You
will need to remove the extraneous whitespace manually where-ever the compiler complains about it. **Note that
you will need administrator privileges to edit these files.**

Running Compiled Programs
-------------------------

**After you've compiled these projects, you will need administrator privileges to run or debug them!**