echo '<RCC>' > resources.qrc
echo '    <qresource prefix="/">' >> resources.qrc
find res -type f | sort | while read -r line; do
    echo "        <file>$line</file>" >> resources.qrc
done
echo '    </qresource>' >> resources.qrc
echo '</RCC>' >> resources.qrc
