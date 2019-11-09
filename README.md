# terminal-c-implementation
c imlemnetation of terminal and distributed terminal
cluster shell and cluster client as basically a single software running on one single machine to facilitate bidirectional functioning

both programs take config_filename and name of node as two arguments

the third program is a standalone terminal implemented in c with all commands of terminal working including pipelining of commands it has beeen implemented by using execv function of c and using child process cretaed by fork and by redirecting standard input and output to pipes as accordingly in  a program
