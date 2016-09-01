#import "AnalysisView.h"
#import "Connection.h"
#import "GuiTree.h"
#import "Inlet.h"
#import "Node.h"
#import "Outlet.h"
#import "RbData.h"
#import "RbDataCell.h"
#import "RbGuiHelper.h"
#import "RbTaxonData.h"
#import "RevBayes.h"
#import "ToolData.h"
#import "ToolParsimony.h"
#import "ToolTreeSet.h"
#import "GuiTree.h"
#import "WindowControllerParsimony.h"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <map>
#include <sstream>
#include <vector>



@implementation ToolParsimony

@synthesize searchMethod;
@synthesize hsSwap;
@synthesize hsKeep;
@synthesize hsMulTrees;
@synthesize hsRearrLimit;
@synthesize hsReconLimit;
@synthesize hsNBest;
@synthesize hsRetain;
@synthesize hsAllSwap;
@synthesize hsUseNonMin;
@synthesize hsSteepest;
@synthesize hsNChuck;
@synthesize hsChuckScore;
@synthesize hsAbortRep;
@synthesize hsRandomize;
@synthesize hsAddSeq;
@synthesize hsNReps;
@synthesize hsHold;
@synthesize bbKeep;
@synthesize bbMulTrees;
@synthesize bbUpBound;
@synthesize bbAddSeq;
@synthesize exKeep;

- (void)closeControlPanelWithCancel {

    [NSApp stopModal];
	[controlWindow close];
}

- (void)closeControlPanelWithOK {

    [NSApp stopModal];
	[controlWindow close];

    if ( [self hasParents] == NO )
        return;

    [self startProgressIndicator];
    [self setStatusMessage:@"Performing parsimony analysis"];

    [NSThread detachNewThreadSelector:@selector(performToolTask)
                       toTarget:self
                     withObject:nil];
}

- (void)encodeWithCoder:(NSCoder*)aCoder {
    
	[super encodeWithCoder:aCoder];
}

- (BOOL)execute {

    BOOL isSuccessful = [self paupSearch];
    return isSuccessful;
}

- (id)init {

    self = [self initWithScaleFactor:1.0];
    return self;
}

- (id)initWithScaleFactor:(float)sf {

    if ( (self = [super initWithScaleFactor:sf]) ) 
		{
        // initialize values
        [self setSearchMethod:HEURISTIC];
        [self setHsSwap:@"TBR"];
        [self setHsKeep:@"No"];
        [self setHsMulTrees:@"Yes"];
        [self setHsRearrLimit:@"None"];
        [self setHsReconLimit:@"Infinity"];
        [self setHsNBest:@"All"];
        [self setHsRetain:@"No"];
        [self setHsAllSwap:@"No"];
        [self setHsUseNonMin:@"No"];
        [self setHsSteepest:@"No"];
        [self setHsNChuck:0];
        [self setHsChuckScore:@"No"];
        [self setHsAbortRep:@"No"];
        [self setHsRandomize:@"AddSeq"];
        [self setHsAddSeq:@"Simple"];
        [self setHsNReps:10];
        [self setHsHold:@"1"];
        [self setBbKeep:@"No"];
        [self setBbMulTrees:@"No"];
        [self setBbUpBound:0];
        [self setBbAddSeq:@"Furthest"];
        [self setExKeep:@"No"];

		// initialize the tool image
		[self initializeImage];
        [self setImageWithSize:itemSize];
        bestTrees = [[NSMutableArray alloc] init];
        treeSetContainers = [[NSMutableArray alloc] init];
		
		// initialize the inlet/outlet information
		[self addInletOfColor:[NSColor greenColor]];
		[self addOutletOfColor:[NSColor redColor]];
        [self setInletLocations];
        [self setOutletLocations];
            
		// initialize the control window
		controlWindow = [[WindowControllerParsimony alloc] initWithTool:self];
		}
    return self;
}

- (id)initWithCoder:(NSCoder*)aDecoder {

    if ( (self = [super initWithCoder:aDecoder]) ) 
		{
        // initialize the tool image
		[self initializeImage];
        [self setImageWithSize:itemSize];
        bestTrees = [[NSMutableArray alloc] init];
        treeSetContainers = [[NSMutableArray alloc] init];
            
        // initialize the control window
		controlWindow = [[WindowControllerParsimony alloc] initWithTool:self];
		}
	return self;
}

- (void)initializeImage {

    itemImage[0] = [NSImage imageNamed:@"Tool_Parsimony25.icns"];
    itemImage[1] = [NSImage imageNamed:@"Tool_Parsimony50.icns"];
    itemImage[2] = [NSImage imageNamed:@"Tool_Parsimony75.icns"];
    itemImage[3] = [NSImage imageNamed:@"Tool_Parsimony100.icns"];
    itemImage[4] = [NSImage imageNamed:@"Tool_Parsimony125.icns"];
    itemImage[5] = [NSImage imageNamed:@"Tool_Parsimony150.icns"];
    itemImage[6] = [NSImage imageNamed:@"Tool_Parsimony200.icns"];
    itemImage[7] = [NSImage imageNamed:@"Tool_Parsimony400.icns"];

	float s[8] = { 0.25, 0.50, 0.75, 1.0, 1.25, 1.50, 2.0, 4.0 };
	for (int i=0; i<8; i++)
        [itemImage[i] setSize:NSMakeSize(ITEM_IMAGE_SIZE*s[i], ITEM_IMAGE_SIZE*s[i])];
}

- (void)paupFinished:(NSString*)paupDirectory {

    if (numberErrors == 0)
        {
        // read trees
        RbGuiHelper* helper = [[RbGuiHelper alloc] init];
        NSMutableArray* myTrees = [helper readTreesFromFile:paupDirectory];
        for (unsigned i=0; i<[myTrees count]; i++)
            {
            // TO DO: We should pass a copy of the tree to the Tree set in case there are multiple treesets downstream of this tool
            for (size_t ts=0; ts<[treeSetContainers count]; ts++)
                {
                ToolTreeSet* treeSet = [treeSetContainers objectAtIndex:ts];
                [self sendTree:(GuiTree*)[myTrees objectAtIndex:i] toTreeSet:treeSet];
                }
            if (i == 0)
                {
                for (size_t ts=0; ts<[treeSetContainers count]; ts++)
                    {
                    ToolTreeSet* treeSet = [treeSetContainers objectAtIndex:ts];
                    [treeSet setOutgroupName:[(GuiTree*)[myTrees objectAtIndex:i] outgroupName]];
                    }
                }
            }

        [self removeFilesFromTemporaryDirectory];
        [self stopProgressIndicator];
        [self setStatusMessage:@""];
        }
    else
        {
        [self stopProgressIndicator];
        [self setStatusMessage:@""];
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Error Aligning Sequences"];
        [alert setInformativeText:@"One or more errors occurred while aligning sequences."];
        [alert runModal];
        }
}

- (BOOL)paupSearch {

    numberErrors = 0;
    
    // find the parent of this tool, which should be an instance of ToolData
    ToolData* dataTool = nil;
    for (size_t i=0; i<[inlets count]; i++)
        {
        Inlet* theInlet = [inlets objectAtIndex:i];
        for (int j=0; j<[theInlet numberOfConnections]; j++)
            {
            Connection* c = [theInlet connectionWithIndex:j];
            Tool* t = [[c outlet] toolOwner];
            if ( [t isKindOfClass:[ToolData class]] == YES )
                dataTool = (ToolData*)t;
            }
        }
    if ( dataTool == nil )
        return NO;

    // calculate how many aligned data matrices exist
    NSMutableArray* alignedData = [NSMutableArray arrayWithCapacity:1];
    for (size_t i=0; i<[dataTool numDataMatrices]; i++)
        {
        if ( [[dataTool dataMatrixIndexed:i] isHomologyEstablished] == YES )
            [alignedData addObject:[dataTool dataMatrixIndexed:i]];
        }
        
    // get a pointer to the single RbData object
    RbData* d = nil;
    if ( [alignedData count] == 0 )
        {
        return NO;
        }
    else if ( [alignedData count] == 1 )
        {
        d = [alignedData objectAtIndex:0];
        }
    else
        {
        return NO;
        }
        
    // check to see if a tree container is downstream of this tool. If so, then purge it of trees
    [treeSetContainers removeAllObjects];
    for (size_t i=0; i<[outlets count]; i++)
        {
        Outlet* theOutlet = [outlets objectAtIndex:i];
        for (int j=0; j<[theOutlet numberOfConnections]; j++)
            {
            Connection* c = [theOutlet connectionWithIndex:j];
            Tool* t = [[c inlet] toolOwner];
            if ( [t isKindOfClass:[ToolTreeSet class]] == YES )
                {
                [treeSetContainers addObject:t];
                if ( [(ToolTreeSet*)t numberOfTreesInSet] > 0 )
                    [(ToolTreeSet*)t removeAllTreesFromSet];
                }
            }
        }
                
    // remove all of the files from the temporary directory
    [self removeFilesFromTemporaryDirectory];
    
    // and make a temporary directory to contain the alignments
    NSString* temporaryDirectory = NSTemporaryDirectory();

    // write the data to the temporary directory
    NSString* modifiedFileName = [NSString stringWithString:[d name]];
    modifiedFileName = [modifiedFileName stringByReplacingOccurrencesOfString:@" " withString:@"_"];

    NSString* dFilePath = [NSString stringWithString:temporaryDirectory];
              dFilePath = [dFilePath stringByAppendingString:modifiedFileName];
              dFilePath = [dFilePath stringByAppendingString:@".nex"];
    [d writeToFile:dFilePath];
    
    // write the file with the nexus commands to the same file
    // write the data to the temporary directory
    NSString* nFilePath = [NSString stringWithString:temporaryDirectory];
              nFilePath = [nFilePath stringByAppendingString:@"nexcmds.nex"];
    NSString* tFilePath = [NSString stringWithString:temporaryDirectory];
              tFilePath = [tFilePath stringByAppendingString:@"ptrees.trees"];
    NSString* cmds = @"#NEXUS\n";
    cmds = [cmds stringByAppendingString:@"begin paup;\n"];
    cmds = [cmds stringByAppendingString:@"set AutoClose=yes WarnReset=no Increase=auto Criterion=parsimony NotifyBeep=no ErrorBeep=no WarnTSave=no;\n"];
    cmds = [cmds stringByAppendingFormat:@"execute %@;", dFilePath];
    //cmds = [cmds stringByAppendingString:@"pset Collapse=no;\n"];
    if (searchMethod == EXHAUSTIVE)
        {
        cmds = [cmds stringByAppendingFormat:@"alltrees keep=%@;\n", exKeep];
        }
    else if (searchMethod == BANDB)
        {
        cmds = [cmds stringByAppendingFormat:@"bandb keep=%@ multrees=%@ addseq=%@ upbound=%1.3lf;\n", bbKeep, bbMulTrees, bbAddSeq, bbUpBound];
        }
    else
        {
        cmds = [cmds stringByAppendingFormat:@"hsearch keep=%@ swap=%@ multrees=%@ RearrLimit=%@ ReconLimit=%@ NBest=%@ Retain=%@ AllSwap=%@ UseNonMin=%@ Steepest=%@ NChuck=%d ChuckScore=%@ AbortRep=%@ Randomize=%@ AddSeq=%@ NReps=%d Hold=%@;\n",
                hsKeep, hsSwap, hsMulTrees, hsRearrLimit, hsReconLimit, hsNBest, hsRetain, hsAllSwap, hsUseNonMin, hsSteepest, hsNChuck, hsChuckScore, hsAbortRep, hsRandomize, hsAddSeq, hsNReps, hsHold];
        //cmds = [cmds stringByAppendingString:@"hsearch;"];
        }
    //cmds = [cmds stringByAppendingString:@"deroottrees;\n"];
    cmds = [cmds stringByAppendingFormat:@"savetrees file=%@ format=nexus replace=yes;\n", tFilePath];
    cmds = [cmds stringByAppendingString:@"quit;\n"];
    cmds = [cmds stringByAppendingString:@"end;\n"];
    [cmds writeToFile:nFilePath atomically:YES encoding:NSASCIIStringEncoding error:nil];
    NSLog(@"cmds=%@", cmds);
    
    // get the path to the paup binary
    NSString* paupPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"paup"];
    
    // set the paup argument
    NSString* paupArg = [NSString stringWithString:nFilePath];
    NSArray* paupArguments = [NSArray arrayWithObjects:paupArg,nil];

    // create the task and launch
    // allocate a task for clustal
    NSTask* paupTask = [[NSTask alloc] init];
    NSPipe* outputPipe = [[NSPipe alloc] init];
    [paupTask setCurrentDirectoryPath:temporaryDirectory];
    [paupTask setLaunchPath:paupPath];
    [paupTask setArguments:paupArguments];
    NSFileHandle* outputFileHandle = [outputPipe fileHandleForReading];
    [paupTask setStandardOutput:outputPipe];

    // launch the task and wait
    [paupTask launch];

    [outputFileHandle waitForDataInBackgroundAndNotify];

    // read the fasta file output
    NSData* inData = nil;
    while ( (inData = [outputFileHandle availableData]) && [inData length] )
        {
        NSString* msg = [[NSString alloc] initWithData:inData encoding:NSASCIIStringEncoding];
        NSLog(@"%@", msg);
        }

    [paupTask waitUntilExit];
    int status = [paupTask terminationStatus];
    if (status != 0)
        numberErrors++;

    // read the alignments on the main thread to prevent errors on graphics.
    [self performSelectorOnMainThread:@selector(paupFinished:)
                         withObject:tFilePath
                      waitUntilDone:NO];

    return YES;
}

- (BOOL)performToolTask {

    return [self paupSearch];
}

- (NSMutableAttributedString*)sendTip {

    NSString* myTip = @" Parsimony Tool ";
    if ([self isResolved] == YES)
        myTip = [myTip stringByAppendingString:@"\n Status: Resolved "];
    else 
        myTip = [myTip stringByAppendingString:@"\n Status: Unresolved "];
    if ([self isFullyConnected] == YES)
        myTip = [myTip stringByAppendingString:@"\n Fully Connected "];
    else 
        myTip = [myTip stringByAppendingString:@"\n Missing Connections "];

    NSDictionary *attr = [NSDictionary 
                 dictionaryWithObjects:[NSArray arrayWithObjects:[NSFont fontWithName:@"Lucida Grande Bold" size:14.0], [[NSColor whiteColor] colorWithAlphaComponent:1.0], nil] 
                               forKeys: [NSArray arrayWithObjects:NSFontAttributeName,NSForegroundColorAttributeName, nil]];

    NSMutableAttributedString* attrString = [[NSMutableAttributedString alloc] initWithString:myTip attributes:attr];

    return attrString;
}

- (BOOL)sendTree:(GuiTree*)t toTreeSet:(ToolTreeSet*)treeSet {
    
    if (treeSet != nil)
        {
        [treeSet addTreeToSet:t];
        return YES;
        }
    return NO;
}

- (void)showControlPanel {

    NSPoint p = [self originForControlWindow:[controlWindow window]];
    [[controlWindow window] setFrameOrigin:p];
	[controlWindow showWindow:self];    
	[[controlWindow window] makeKeyAndOrderFront:nil];
    [NSApp runModalForWindow:[controlWindow window]];
}

- (NSString*)toolName {

    return @"Parsimony";
}

- (void)updateForChangeInParent {

    isResolved = YES;
    
    // find the parent of this tool, which should be an instance of ToolData
    ToolData* dataTool = nil;
    for (int i=0; i<[inlets count]; i++)
        {
        Inlet* theInlet = [inlets objectAtIndex:i];
        for (int j=0; j<[theInlet numberOfConnections]; j++)
            {
            Connection* c = [theInlet connectionWithIndex:j];
            Tool* t = [[c outlet] toolOwner];
            if ( [t isKindOfClass:[ToolData class]] == YES )
                dataTool = (ToolData*)t;
            }
        }
    if ( dataTool == nil )
        isResolved = NO;

    // calculate how many aligned data matrices exist
    NSMutableArray* alignedData = [NSMutableArray arrayWithCapacity:1];
    for (int i=0; i<[dataTool numDataMatrices]; i++)
        {
        if ( [[dataTool dataMatrixIndexed:i] isHomologyEstablished] == YES )
            [alignedData addObject:[dataTool dataMatrixIndexed:i]];
        }
    if ( [alignedData count] != 1 )
        isResolved = NO;
}

@end
