.pragma library

.import Sailfish.Silica 1.0 as Silica

function thumbnailSize(originalsize)
{
    var scaledw = 0, scaledh = 0, padding = Silica.Theme.paddingLarge;

    // Check if the image fits to screen
    if((originalsize.w < (Silica.Screen.width - padding)) && (originalsize.h < (Silica.Screen.height - padding)))
        return originalsize;

    // Scale the image
    if(originalsize.w > originalsize.h) // Landscaped image
    {
        scaledw = Math.max(Silica.Screen.width, originalsize.w) - padding;
        scaledh = (originalsize.h / originalsize.w) * scaledw;
    }
    else // Portait image
    {
        scaledh = Math.max(Silica.Screen.height, originalsize.h) - padding;
        scaledw = (originalsize.w / originalsize.h) * scaledh;
    }

    return Qt.size(scaledw, scaledh);
}
