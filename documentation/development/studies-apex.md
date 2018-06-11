Structure of studies within APEX
================================

This page documents the implementation details of studies within APEX. Starting
from the lowest layer.

ManagedDirectory
----------------

This is a directory which is also a git repository. It provides a simplified
interface to add, push, and pull files. It uses libgit2 as git
implementation. All the expensive operations (push, pull, fetch, and checkout)
are done on a separate thread, but are optionally synchronous through the use of
different connection types. Pass `Qt::BlockingQueuedConnection` to make them
synchronous.

Study
-----

There are two types of study, public and private. A public study has got a
single ManagedDirectory for the experiments. A private study has got an extra
ManagedDirectory for the results.

Furthermore it provides some convenience functions to easily query the
experiments.

StudyManager
------------

This class ties it all together and provides the interface for anything study
related to the rest of APEX. It has several responsibilities.

It has a global instance which should be used for all networking related
functions[^1]. That is functions which update results or experiments. Note that
networking related functions are not guaranteed to be reentrant depending on the
transport protocol[^2]. Functions that only query the settings object (and the
object's own data) are reentrant.

In short, use the global instance from a single thread, it will automatically do
the expensive operations on a different thread[^3].

#### 1. Storing studies

It uses QSettings to keep a list of linked studies and the current active
study. It updates the settings each time this list gets altered.

#### 2. Experiment life cycle

It ties into the experiment life cycle as well. When a new experiment is started
`StudyManager::newExperiment` is called from `ApexControl::newExperiment`. If
the experiment is part of the active study and if the active study is private,
this first function will return a path based on the experiment path. The
returned path should be set as the result file path, because result collection
depends on it. If the study is public or the experiment does not belong to the
active study it will return an empty string, which can be ignored.

After the experiment `StudyManager::afterExperiment` is called from
`StudyModule`. This will add the result to the study's result repository and try
to push it to the server if the result belongs to the active study and if it is
private.

#### 3. Dialog

It also provides a dialog through `StudyManager::showConfigurationDialog` which
provides an interface for linking new studies, deleting studies, starting a
study, setting the active study, etc...

#### 4. Internal state

The StudyManager has an internal state for the networking operations. It will
refuse to do any network operation as long as one is in progress. After starting
a network operation, wait for the `StudyManager::studiesUpdated` signal before
starting another.

StudyRunner
-----------

When an active study is set and APEX is started with `--pluginrunner study`[^4],
it will load the index experiment of said study, or provide a list of all
experiments in the study if no index was found.

StudyModule
-----------

This is an ApexModule which provides some meta information about the study for
the results file, and which will call `StudyManager::afterExperiment`.

---

##### Notes

[^1]: Optionally, local instances can be used to query anything settings related
    (active study, getting a list of studies, seeing if a study belongs to an
    active study, ...). But there's no benefit in doing so.

[^2]: Using HTTP and SSH transport protocols should be thread-safe. On Windows
    HTTPS is also thread-safe. HTTPS on Linux and Android depends entirely on
    the thread safety of OpenSSL:

    Starting with OpenSSL 1.1.0, the network operations of StudyManager should be
    thread-safe. And as a result StudyManager should be reentrant (but not
    thread-safe).

    Note that reentrancy doesn't guarantee any ordering on git operations
    should you open the same git repo from different threads.

[^3]: The only exception to this is `StudyManager::fetchRemoteBranches` as it
    uses `ManagedDirectory::lsRemote`. It's only used during the linking
    process, but there's room for improvement there.

[^4]: On Android this can be achieved through making a shortcut to the runner.
