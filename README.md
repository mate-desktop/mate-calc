# MATE Calculator

![mate-calc-icon](mate-calc.png)

## General Information

MATE Calculator (*mate-calc*) started as a fork of *gnome-calc*, the calculator application
that was previously in the OpenWindows Deskset of the Solaris 8
operating system.

## Calctool history

Calctool was a project I worked on before I joined the OpenWindows
DeskSet engineering group at Sun. It was originally released to
comp.sources.unix in the late 1980's, and worked with many different
graphics packages including SunView, X11, Xview, NeWS and MGR. There
was also a version that worked on dumb tty terminals.

It used a double-precision maths library that was a combination of the
work of Fred Fish and various routines that were in the BSD 4.3 maths
library.

A lot of people in the community provided feedback in the form of
comments, bug reports and fixes. In 1990, I started working in the
DeskSet engineering group. I was working for Sun Microsystems in
Australia at the time, (having moved there from England in 1983).

I searched around looking for multiple precision maths libraries and
found a package called MP written in FORTRAN by Richard Brent. I
converted it to C, adjusted the glue between the resultant code and the
calctool code, and this went on to be the basis of the calculator that
was in the OpenWindows DeskSet. I also added scientific, financial and
logical modes. This calctool was also the basis of the dtcalc
application that is a part of CDE (albeit I had nothing to do with
that).

With its inclusion in the MATE CVS repository, it was renamed to
*mate-calc*.

More recently, Sami Pietila provided arithmetic precedence support and
Robert Ancell converted the UI to use Glade.

## Build/Installation

MATE Calculator requires GTK+ (>= 3.22) and the [GNU MPFR](https://www.mpfr.org/) and [GNU MPC](http://www.multiprecision.org/mpc) libraries. For a complete list of dependencies see the [build.yml](https://github.com/mate-desktop/mate-calc/blob/master/.build.yml).

Simple install procedure:

```
$ ./autogen.sh                              # Build configuration
$ make                                      # Build
[ Become root if necessary ]
$ make install                              # Installation
```


## Acknowledgements

See the [AUTHORS](https://github.com/mate-desktop/mate-calc/blob/master/AUTHORS) file.

Suggestions for further improvement would be most welcome, plus bug
reports and comments.

The Mate Team.