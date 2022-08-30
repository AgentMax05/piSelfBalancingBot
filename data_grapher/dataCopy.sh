echo copying data file from raspberry pi

fileDestination=$(pwd)

echo enter raspberry pi ip address: 
read piAddress
echo enter file location on raspberry pi: 
read fileLocation
echo IP: $piAddress
echo file location: $fileLocation
echo file destination: $fileDestination/

read -p "Confirm? [y/n] " -n 1 -r
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo 
    scp pi@$piAddress:$fileLocation "$fileDestination"
fi