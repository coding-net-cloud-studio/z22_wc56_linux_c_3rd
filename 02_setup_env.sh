#!/usr/bin/env bash

f36_install_gcc_10_versioin(){
  apt install -y gcc-10
  apt install -y g++-10

  [ -f /usr/bin/gcc-9 ]  && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 120
  [ -f /usr/bin/gcc-10 ] && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 150
  [ -f /usr/bin/g++-9 ]  && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 120
  [ -f /usr/bin/g++-10 ] && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 150

  return 0
}

l22_apt_install_software(){
  sudo apt update

  # NOTE ch07等多个章节
  # /usr/bin/ld: cannot find -lgdbm_compat
  sudo apt update
  sudo apt install -y libgdbm-dev
  sudo apt install -y libgdbm-compat-dev

  # NOTE ch08章节需要的
  sudo apt install libmysqlclient-dev

  # NOTE ch10章节需要的
  sudo apt-get -y install electric-fence

  # NOTE ch12章节需要的
  sudo apt-get -y install libpthread-dev

  return 0
}

main(){

  f36_install_gcc_10_versioin

  l22_apt_install_software

  return 0
}

main

