#!/usr/bin/env bash

f23_install_some_software(){
	if [[ -f $(which cloudstudio) ]]; then
		apt update -y
		DEBIAN_FRONTEND=noninteractive apt install -y \
			iputils-ping \
			make \
			lsof \
			net-tools \
			psmisc \
			file \
			rsync \
			curl \
			direnv \
			sqlite3 \
			libsqlite3-dev \
			bats \
			jq \
			gron \
			pdfgrep \
			ripgrep \
			httpie \
			duff \
			mlocate \
      pv

	fi

	return 0
}

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
  sudo apt install -y libgdbm-dev
  sudo apt install -y libgdbm-compat-dev

  # NOTE ch08章节需要的
  sudo apt install -y libmysqlclient-dev

  # NOTE ch10章节需要的
  sudo apt install -y electric-fence

  # NOTE ch12章节需要的
  # sudo apt-get -y install libpthread-dev

  return 0
}

f94_2828_30_main(){

	if [[ -f $(which cloudstudio) ]]; then

    f23_install_some_software

    f36_install_gcc_10_versioin

    l22_apt_install_software

  fi

  return 0
}

main(){

  f94_2828_30_main

  return 0
}

# 下面是_正式_的入口
[ -z "$1" ] && eval f94_2828_30_main || eval $1 $*
