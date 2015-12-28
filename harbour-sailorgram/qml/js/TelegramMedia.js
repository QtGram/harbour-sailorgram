.pragma library

.import Sailfish.Silica 1.0 as Silica

function thumbnailSize(originalsize, isportrait)
{
    var padding = Silica.Theme.paddingLarge * 4;

    // Check if the image fits to screen
    if((originalsize.width < (Silica.Screen.width - padding)) && (originalsize.height < (Silica.Screen.height - padding)))
        return originalsize;

    // Scale the image
    var scaledw = Math.min(isportrait ? Silica.Screen.width : Silica.Screen.height, originalsize.width) - padding;
    var scaledh = (originalsize.height / originalsize.width) * scaledw;
    return Qt.size(scaledw, scaledh);
}
