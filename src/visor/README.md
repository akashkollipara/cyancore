# Visor

This folder will contain all the sources for workers and services that can be provided by a visor.

Its upto user's imagination to use the api's in such a way that they can craft out the visor as per their liking.

User's are free to write their own OS, hypervisor, terravisor and user space apps based on their requriements. To do this, the user needs to specify the execution mode in the build.mk of the project file. This is necessary to assist visor's sources to integrate correct arch functions. _Note: Onus is on cyancore's developer to be careful regaring naming convention and backward compatibilty of such functions_. Going further, to enable access control, users/developers should make best use of libresources. This can be a powerful tool to build a robust software stack.
