From rushmash/gcc-arm-embedded-docker@sha256:affe1564eef00020791c65e75042c3ae04b7d8f0cdd7b51b360b46550fa82f2b
#the source file folder must be linked to the following folder on the container
WORKDIR /usr/src/app/
# Resulting build files are located 
ENTRYPOINT cmake -GNinja -Bbuild . && ninja -Cbuild && chmod -R 777 build
