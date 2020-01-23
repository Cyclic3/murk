#!/bin/sh
set -e

export AUTOSTAGE_DIR=$(dirname $(readlink -f "${0}"))
export SOURCE_BIN=$(readlink -f "$1")
export TARGET_DIR=$(readlink -f "$2")

cd ${3:-.}

case $2 in
        x64)
                export APPRUN_URL=https://github.com/AppImage/AppImageKit/releases/download/continuous/AppRun-x86_64
                export LD_SO_PATH=/lib64/ld-linux-x86-64.so.2
		;;
        x86)
                export APPRUN_URL=https://github.com/AppImage/AppImageKit/releases/download/continuous/AppRun-i686
                export LD_SO_PATH=/lib/ld-linux.so.2
		;;
	*)
                echo "Usage: $0 executable arch [destination]"
		echo
		echo "Arches: x64 x86"
		exit 1;
		;;
esac
#curl -L $APPRUN_URL -o AppRun
chmod +x AppRun

cat << EOF > automurk.desktop
[Desktop Entry]
Encoding=UTF-8
Version=1.0
Type=Application
Terminal=true
Exec=start.sh
Name=automurk
Icon=empty
Categories=Utility;
EOF
cp $AUTOSTAGE_DIR/empty.png .

mkdir -p usr/bin
cd usr/bin

cat << EOF > start.sh
#!/bin/sh
export LIB_PATH=\$(dirname \$(readlink -f "\${0}"))
\$LIB_PATH/ld.so --library-path "\$LIB_PATH" \$LIB_PATH/$(basename $1) \$@
EOF

chmod +x start.sh

cp $SOURCE_BIN .

export FILES="$(ldd $1 | awk '{if (NF >= 3) print $3}')"

ldd $1 | awk '{if (NF >= 3) print $3}' | while read file
do
        cp $file .
done

cp "$LD_SO_PATH" ld.so
