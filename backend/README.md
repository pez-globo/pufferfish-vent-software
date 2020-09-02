# backend

## Getting Started

### Setting Up Pyenv

It is recommended to use [pyenv](https://github.com/pyenv/pyenv) to manage
your local virtual environments.

* Install [pyenv](https://github.com/pyenv/pyenv) - it is recommended to use the automatic [pyenv-installer](https://github.com/pyenv/pyenv-installer) to do this. Note that you may still need to set up environment variables, as described in the pyenv repo's README.md, after running the automatic installer.
* Install [pyenv-virtualenv](https://github.com/pyenv/pyenv-virtualenv). Note that the automatic installer should install this for you.

If you're on Ubuntu 18.04, you will need to install some dependencies for pyenv
to build Python 3.7.7 correctly:

```bash
sudo apt install libffi-dev
```

Create a new local environment for Python 3.7.7:

```bash
$ pyenv install 3.7.7
$ pyenv virtualenv 3.7.7 ventserver
$ pyenv
```

Note: If you would like to configure your working directory to always use
your `ventserver` virtual environment, do:

```bash
$ cd /path/to/vent-dev/backend
$ pyenv local ventserver
```

This will automatically activate your virtual environment when you are in
that directory.

### Setting Up Poetry

Poetry ([poetry](https://python-poetry.org/docs/)) is used for dependency
management of this project.  You will need to follow the installation steps
on their site to install it.

### Installing Dependencies

To install all dependencies do: `poetry install`

### Executing Application

Currently there is no application to execute.

## Development

### Linting

Linting is a great way to ensure code quality and style remain consistent
between developers.  This project uses pylint with
[Google pystyle](https://github.com/google/styleguide/blob/gh-pages/pyguide.md)
rules and will check whether the code adheres to these rules during a pull request.

To improve development time, you can configure your IDE to run pyLint as
you are developing. Here are a few links to linting integration plugins for a few
common IDEs:

* Sublime: https://packagecontrol.io/packages/Pylinter
* Atom: https://atom.io/packages/linter-pylint
* Vim: https://github.com/dense-analysis/ale

To run the pylint commands, do:

```bash
(ventserver) $ poetry run pylint ventserver
```

### Type-Checking

Type hints are a great way to help document and automatically check code. To run
type-checking, do:

```bash
(ventserver) $ poetry run mypy ventserver
```

### Testing

Unit testing is done utilizing [pytest](https://docs.pytest.org/en/latest/) and
[tox](https://tox.readthedocs.io/en/latest/) for automation.

To execute the test suite, run:

```bash
(ventserver) $ poetry run tox
```

Afterwards, you can quickly re-run tests without running the full tox process:

```bash
(ventserver) $ poetry run coverage run --parallel -m pytest --basetemp=.tox/py37/tmp --verbose tests
```
