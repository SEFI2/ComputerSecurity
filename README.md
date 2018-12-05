This is the repo for labs @ CSE481-2018-Fall.

The following commands should lead you to VM's shell

```
vagrant up
vagrant ssh
```

If your VM is too slow when working on the lab 3, run the following commands.

Install a plugin.
```
vagrant plugin install vagrant-vbguest
```


Install the updated guest extension

```
vagrant vbguest --do install
```

In between the two commands, you may want to copy your ssh key into
the VM by running:
./scripts/push-key.sh <your-private-key>

if it doesn't work, fix the port number (2222) in the script from
the one you can find by 
```
vagrent port
```

If you are not familiar with it already, `tmux` will make your life easier
when you are in the VM's shell.


If you are working on windows, you should be able to use similar commands like:

```
vagrant.exe up
vagrant.exe ssh
```
