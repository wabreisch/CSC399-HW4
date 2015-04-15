## C System Library Notes

### [fork](http://man7.org/linux/man-pages/man2/fork.2.html)
* fork() creates a new process by duplicating the calling process
* The new process is referred to as the child process
* The calling process is referred to as the parent process
* [Nice article on how to use fork()](http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/create.html)

### [dup](http://man7.org/linux/man-pages/man2/dup.2.html)
* Used to duplicate a file descriptor, which indicates access status to a particular file (stdin, stdout, stderr)

### [chdir](http://man7.org/linux/man-pages/man2/chdir.2.html)
* **Done**

### [getenv](http://man7.org/linux/man-pages/man3/getenv.3.html)

### [pipe](http://man7.org/linux/man-pages/man2/pipe.2.html)
* [Nice pipe article](http://linux.die.net/man/2/pipe) (which makes use of fork() as well)
* [Good article on pipes and file descriptors here](http://pubs.opengroup.org/onlinepubs/009695399/functions/pipe.html)


### Pipes
* `a |` induces a broken pipe error
* `| b` induces a missing command error
* `a | b` should operate properly