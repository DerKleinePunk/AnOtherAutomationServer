#!/bin/bash
# wget -O build.sh https://raw.githubusercontent.com/DerKleinePunk/AnOtherAutomationServer/master/build.sh
# chmod +x build.sh

FILE=logger.sh
if [ ! -f "$FILE" ]; then
    wget -O logger.sh https://raw.githubusercontent.com/DerKleinePunk/AnOtherAutomationServer/master/logger.sh
fi

source ./logger.sh
SCRIPTENTRY

echo try to download and build IoBackend and Dependecy
INFO "try to download and build IoBackend and Dependecy"
reproBuild="false"
if [ -d .git ]; then
	echo "called inside repro"
	reproBuild="true"
fi

echo "reproBuild $reproBuild"
INFO "reproBuild $reproBuild"

if [ "$reproBuild" = "false" ] ; then
	wget -O DebianPackages.txt https://raw.githubusercontent.com/DerKleinePunk/AnOtherAutomationServer/master/DebianPackages.txt
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
		echo "wget give an error"
		exit $exitCode
	fi
else
	echo "Don't forget git pull bevor building"
	INFO "Don't forget git pull bevor building"
fi

DEPENSFILE="DebianPackages.txt"

InstallPackage(){
	packageName="$1"
	PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $packageName|grep "install ok installed")
	DEBUG "Checking for $packageName: $PKG_OK"
	if [ "" = "$PKG_OK" ]; then
		echo "No $packageName. Setting up $packageName."
		DEBUG "No $packageName. Setting up $packageName."
		sudo apt-get --yes install $packageName
	fi
}

InstallSDLComponent(){
	packageName="$1"
	packageVersion="$2"
	if [ ! -d "$packageVersion" ]; then
		wget -N https://www.libsdl.org/projects/$packageName/release/$packageVersion.tar.gz
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
		   echo "wget give an Error"
		   exit $exitCode
		fi
		tar -xzf $packageVersion.tar.gz
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
		   echo "tar give an Error"
		   exit $exitCode
		fi
		cd $packageVersion
		./autogen.sh
		./configure
		make -j$(nproc)
		sudo make install
		cd ..
	else
		cd $packageVersion
		./autogen.sh
		./configure
		make -j$(nproc)
		sudo make install
		cd ..
	fi
}

sudo apt-get --yes update
sudo apt-get --yes upgrade
sudo apt-get autoremove -y
#sudo apt-get purge -y libsdl2-dev
#sudo apt-get purge -y libsdl2-2.0-0
sudo apt-get autoremove -y

rpiversion=$(cat /sys/firmware/devicetree/base/compatible |cut -c1-13)
echo "PI $rpiversion"
DEBUG "PI $rpiversion"

InstallPackage git
InstallPackage git-lfs
InstallPackage cmake

while read LINE; do
     InstallPackage $LINE
done < $DEPENSFILE

if [ "$reproBuild" = "true" ] ; then
  cd ..
fi

echo building IoBackend and Dependecy
echo "we are here"
pwd

DIRECTORY="AnOtherAutomationServer"
if [ ! -d "$DIRECTORY" ]; then
	git clone https://github.com/DerKleinePunk/AnOtherAutomationServer.git
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
	DEBUG "Repro Clone done"
	cd $DIRECTORY
	git submodule init
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
	git submodule update
	if [ $exitCode -ne 0 ] ; then
	   echo "git give an Error"
	   exit $exitCode
	fi
	DEBUG "git submodule done"
else
	cd $DIRECTORY
	if [ "$reproBuild" = "false" ] ; then
		git pull
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
			echo "git give an Error"
			exit $exitCode
		fi
	fi
	if [ ! -f modules/PiGPIOHelper/LICENSE ]; then
		git submodule init
		exitCode=$?
		if [ $exitCode -ne 0 ] ; then
	   		echo "git give an Error"
	   		exit $exitCode
		fi
	fi
	git submodule update
	exitCode=$?
	if [ $exitCode -ne 0 ] ; then
		echo "git give an Error"
		exit $exitCode
	fi
fi

git lfs fetch --all

DIRECTORY="buildrelease"
if [ ! -d "$DIRECTORY" ]; then
	mkdir $DIRECTORY
fi
cd $DIRECTORY

cmake .. -DCMAKE_BUILD_TYPE=Release -DTARGET=Linux -DENABLE_CPPCHECK=OFF
exitCode=$?
if [ $exitCode -ne 0 ] ; then
	echo "cmake give an Error"
	exit $exitCode
fi

cmake --build . -j $(nproc)
exitCode=$?
if [ $exitCode -ne 0 ] ; then
	echo "cmake give an Error"
	exit $exitCode
fi
cd ..

./buildFlutter.sh

cd bin/Linux
#./TestApp.bin -createconfig

SCRIPTEXIT