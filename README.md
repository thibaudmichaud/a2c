# A2C

A2C is a compiler for the algorithmic language taught at EPITA. Specifications for the
language are available in french
[here](http://algo.infoprepa.epita.fr/index.php/Epita:Algo:M%C3%A9mo-Langage).
It translates algorithms to C programs, and the result can be compiled
with any C compiler. For ease of use, you can also try the [web
interface](http://a2c.too.gy).

# Installation
Download and extract the zip file, or clone the project with git. Then from the
root directory run:

./bootstrap

./configure

make

sudo make install

## Usage

To compile in french version:

./a2c FILE > FILE.c

To compile in english version:

./a2c --en FILE > FILE.c

## MALT

A group of four students at EPITA (French Engineering IT School).

Lucien Boillod (lucienboillod@gmail.com)

Thibaud Michaud (thibaud.michaud@epita.fr)

Maxime Gaudron (maxime.gaudron@epita.fr)

Charles Yaiche (charles.yaiche@epita.fr)

## Documentation

See the documentation section of the website (http://a2c.lucien-boillod.net/src/docs/index.html)

Many examples are available in the tests/ directory.

## Contact List
You can simply send a mail to one of our personal addresses, or contact us via our website (http://a2c.lucien-boillod.net).
You can also send us a pull request on Github, if you want to participate.

If you see any error, please let us know.
