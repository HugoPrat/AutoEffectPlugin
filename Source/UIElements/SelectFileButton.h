/*
  ==============================================================================

    SelectFileButton.h
    Created: 24 Sep 2020 9:27:53am
    Author:  Matthias Demoucron

  ==============================================================================
*/

#pragma once

#ifdef WITH_CMAKE
    #include "CustomJuceHeader.h"
#else
    #include "../JuceLibraryCode/JuceHeader.h"
#endif

class SelectFileButton :
public TextButton,
public FileDragAndDropTarget,
public DragAndDropContainer
{
public:
    SelectFileButton(const String& buttonName): TextButton(buttonName)
    {
        //acceptedTypes = types;
        ///chooserMessage = chooserMess;
        
//        onClick  = [this] {
//
//        };
    }
    
    ~SelectFileButton()    {}
    
    void paint (Graphics& g) override
    {
        TextButton::paint(g);
        
        // draw a red line around the comp if the user's currently dragging something over it..
        if (somethingIsBeingDraggedOver)
        {
            g.setColour (Colours::red);
            g.drawRect (getLocalBounds(), 3);
        }
    }
    
    void showFileChooser() {
        
        File defaultFolder = File::getSpecialLocation(File::userHomeDirectory);
        if (listener != nullptr) {
            String folderPath = listener->selectFileButtonWantsFolderPathForChooser(this);
            if (!folderPath.isEmpty() && File(folderPath).exists()) {
                defaultFolder = File(folderPath);
            }
        }
        
        chooser = std::make_unique<FileChooser> (chooserMessage, defaultFolder, acceptedTypesString, true, false);
        
        int folderChooserFlags = 0;
        
        if (!saveFileMode) {
            if (multipleFilesAllowed) {
                folderChooserFlags = FileBrowserComponent::openMode
                | FileBrowserComponent::canSelectFiles
                | FileBrowserComponent::canSelectMultipleItems;
            } else {
                folderChooserFlags = FileBrowserComponent::openMode
                    | FileBrowserComponent::canSelectFiles;
            }
        } else {
            if (!folderChooser) {
                folderChooserFlags = FileBrowserComponent::saveMode
                | FileBrowserComponent::canSelectFiles
                | FileBrowserComponent::warnAboutOverwriting;
            } else {
                folderChooserFlags = FileBrowserComponent::openMode
                | FileBrowserComponent::canSelectDirectories;
            }
        }

        chooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)
        {
            Array<URL> myFiles;
            
            if (!saveFileMode && !multipleFilesAllowed)
                myFiles.add(chooser.getURLResult());
            else
                myFiles = chooser.getURLResults();
            
            StringArray myFilesPath;
            for (int i=0;i<myFiles.size(); i++) {
                ///When selection cancel, empty URL is returned, have to check
                if (myFiles[i].isEmpty())
                    return;
                myFilesPath.add(myFiles[i].getLocalFile().getFullPathName());
            }
            this->setCheckSelectedFiles(myFilesPath, myFiles);
        });
    }
    
    //==============================================================================
    // Custom methods
    //==============================================================================
    
    void setShowFileChooserOnClick(bool bl) { showFileChooserOnClick = bl;}
    
    bool getShowFileChooserOnClick() { return showFileChooserOnClick;}
    
    void setAcceptedTypes(StringArray types) {
        acceptedTypes = types;
        
        for (int i=0; i<acceptedTypes.size(); i++) {
            if (i>0)
                acceptedTypesString.append(";", 10);
            acceptedTypesString.append("*", 10);
            acceptedTypesString.append(acceptedTypes[i], 10);
        }
    }
    
    void setSelectFolder(bool bl) { folderChooser = bl;}
    
    void setChooserMessage(String mess) {
        chooserMessage = mess;
    }
    
    void setMultipleFilesAllowed(bool bl) {
        multipleFilesAllowed = bl;
    }
    
    void setMessage(String _message)
    {
        message = _message;
        repaint();
    }
    
    StringArray getSelectedFiles() {return validFiles;}
    

    void setCheckSelectedFiles(StringArray files, Array<URL> urls) {
        if (files.size() > 1 && !multipleFilesAllowed) {
            std::cout << "WARNING: SelectFileButton only allows one file at a type. Taking only the first file" << std::endl;
            files.removeRange(1, files.size()-1);
        }
        
        validFiles.clear();
        if (!folderChooser) {
            for (int i=0; i<files.size(); i++) {
                File file(files[i]);
                for (int i2=0; i2<acceptedTypes.size(); i2++) {
                    if (file.getFileExtension().equalsIgnoreCase(acceptedTypes[i2])) {
                        validFiles.add(files[i]);
                        break;
                    }
                }
            }
        } else {
            validFiles.add(files[0]);
        }
        
        if (validFiles.size() > 0 && listener != nullptr) {
            listener->selectFileButtonDidSelectNewFiles(this, validFiles, urls);
        }
    }
    
    //==============================================================================
    // FileDragAndDropTarget
    //==============================================================================
    
    bool isInterestedInFileDrag (const StringArray& files) override
    {
        return canDropFiles;
    }
    
    void fileDragEnter (const StringArray& /*files*/, int /*x*/, int /*y*/) override
    {
        if (canDropFiles && !isDraggingOut) {
            somethingIsBeingDraggedOver = true;
            repaint();
        }
    }
    
    void fileDragMove (const StringArray& /*files*/, int /*x*/, int /*y*/) override
    {
    }
    
    void fileDragExit (const StringArray& /*files*/) override
    {
        if (canDropFiles && !isDraggingOut) {
            somethingIsBeingDraggedOver = false;
            repaint();
        }
    }
    
    void filesDropped (const StringArray& files, int /*x*/, int /*y*/) override
    {
#ifndef MOBILE
        std::cout << "filesDropped" << std::endl;
        if (canDropFiles && !isDraggingOut) {
            somethingIsBeingDraggedOver = false;
            repaint();
            
            std::cout << "files dropped " << files[0] << std::endl;
            
            setCheckSelectedFiles(files, {});
        }
#endif
        //triggerClick();
    }
    
    //==============================================================================
    // MouseEvent::Listener
    //==============================================================================
    
    void mouseEnter(const MouseEvent &event) override
    {
        if (somethingIsBeingDraggedOver) return;
    }
    
    void mouseExit(const MouseEvent &event) override
    {
        if (somethingIsBeingDraggedOver) return;
    }
    
    void mouseDown(const MouseEvent &event) override
    {
        if (!canDragFiles) {
            if (showFileChooserOnClick)
                showFileChooser();
            else {
                if (listener != nullptr)
                    listener->selectFileButtonWasClicked(this);
            }
        }
    }
    
    void mouseUp(const MouseEvent &event) override
    {
        if (canDragFiles && !isDraggingOut) {
            if (showFileChooserOnClick)
                showFileChooser();
            else {
                if (listener != nullptr)
                    listener->selectFileButtonWasClicked(this);
            }
        }
        isDraggingOut = false;
    }
    
    void mouseDrag(const MouseEvent &event) override
    {
        if (!canDragFiles) return;
        
        if (somethingIsBeingDraggedOver) return;
        
        MouseEvent e2 (event.getEventRelativeTo (this));
        if (e2.getLengthOfMousePress()>100 && !isDragAndDropActive()) {
            isDraggingOut = true;
            if (listener)
                listener->selectFileButtonIsDragging(this, e2);
        }
    }
    
    //==============================================================================
    // Listener
    //==============================================================================
    
    class JUCE_API  Listener
    {
    public:
        //==============================================================================
        /** Destructor. */
        virtual ~Listener() = default;
        
        virtual void selectFileButtonWasClicked(SelectFileButton* button){};
        
        virtual void selectFileButtonDidSelectNewFiles(SelectFileButton* button, StringArray files, Array<URL> urls){};

        virtual void selectFileButtonIsDragging(SelectFileButton* button, const MouseEvent &event){};
        
        virtual String selectFileButtonWantsFolderPathForChooser(SelectFileButton* button) {
            return File::getSpecialLocation(File::userHomeDirectory).getFullPathName();
        };
    };
    
    Listener *listener = nullptr;
    
    bool canDropFiles = true;
    bool canDragFiles = false;
    
    bool saveFileMode = false;
    
    
private:
    
    StringArray validFiles;
    
    String chooserMessage;
    
    /** The types of files that can be selected / dropped */
    StringArray acceptedTypes;
    
    String acceptedTypesString;
    
    bool folderChooser = false;
    
    /** Whether we can drop multiple files, or only one at a time */
    bool multipleFilesAllowed = false;
    
    bool showFileChooserOnClick = true;
    
    String message  { ""};
    
    bool somethingIsBeingDraggedOver = false;
    
    bool isDraggingOut = false;
    
    /**Store FIleChooser elsewhere than stack*/
    std::unique_ptr<FileChooser> chooser;
};

