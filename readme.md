C++ Assignments for C programmers
=================================


To facilitate learning C++ as a new language, here are a few assignments
for C programmers to play with.  Each assignment is in its own branch.  Sample
solutions to those assignments are in their own branches as well.  The history
of those branches has been tailored to reflect individual steps that guide
towards the sample solution.  Each assignment has two solution branches:
`assignment-X-solution` and `assignment-X-solution-advanced`.  In the plain
solution branch for an assignment, a solution to the assignment utilizing the
C++ features which are the focus for that assignment are applied.  In the
advanced solution branch for an assignment, the C++ features explored thus
far will be applied to the program to illustrate designs and applications
approaching modern professional C++ usage.  These advanced solutions should
be considered a demonstration of more tasteful application of C++ features
than merely solving the assignment.  In the later assignments, these advanced
solution branches should demonstrate the way that a professional C++ developer
might write the code for that assignment.

The first assignment, `assignment-1` branch, is a simple file copy program.  It
uses C IO primitives but the proposed solution implements C++ RAII types to
manage the lifecycle of C `stdio` resources.

The second assignment, `assignment-2` branch, is a pair of complex number
arithmetic libraries.  One uses external-structure definition to provide
data hiding in C, but requires resource-lifetime management and imposes
function-call overhead.  The other uses inline-structure defintiion
and macros to attain speed, at the cost of ugliness and multiple
evaluation risks.  The proposed solution for students should implement
a single C++ complex number library using inline operator overloads and
optionally use `private` to provide data hiding.  The result should be a
simple-to-use C++ complex number type which looks like a built-in type
in terms of usage.
