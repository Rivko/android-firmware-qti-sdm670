#!/usr/bin/perl
###############################################################################################################################
# Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
###############################################################################################################################

use strict;
use warnings;
use XML::Simple;
use File::Basename;
use File::Path;
use Data::Dumper;

sub GetData
{
    my $xml  = XMLin($_[0], forcearray => [ 'Usecase', 'Pipeline', 'Target', 'Node', 'NodeProperty', 'BufferFlags', 'TargetFormat' , 'DstPort', 'BypassPortSrcId', 'Link']);
    #print Dumper($xml)."\n";
    #exit;
    my $usecases;

    foreach my $usecase (@{$xml->{Usecase}})
    {
        my $localUsecase = {Name             => $usecase->{UsecaseName},
                            StreamConfigMode => $usecase->{StreamConfigMode},
                            targets          => [],
                            pipelines        => [] };

        if ($usecase->{Targets} && $usecase->{Targets}->{Target})
        {
            foreach my $target (@{$usecase->{Targets}->{Target}})
            {
                # Supporting old format
                $target->{MinW} = $target->{Range}->{MinW};
                $target->{MinH} = $target->{Range}->{MinH};
                $target->{MaxW} = $target->{Range}->{MaxW};
                $target->{MaxH} = $target->{Range}->{MaxH};
                push @{$localUsecase->{targets}}, $target;
            }
        }

        foreach my $pipeline (@{$usecase->{Pipeline}})
        {
            my $localPipeline = {Name      => $pipeline->{PipelineName},
                                 nodes     => [],
                                 links     => [],
                                 sinkCount => 0};

            if ($pipeline->{NodesList})
            {
                foreach my $node (@{$pipeline->{NodesList}->{Node}})
                {
                    if ($node->{NodeProperty})
                    {
                        foreach my $prop (@{$node->{NodeProperty}})
                        {
                            $prop->{Name} = 'NodeProperty';
                            push @{$node->{property}}, $prop;
                        }
                    }
                    push @{$localPipeline->{nodes}}, $node;
                }
            }

            if ($pipeline->{PortLinkages} && $pipeline->{PortLinkages}->{Link})
            {
                foreach my $link (@{$pipeline->{PortLinkages}->{Link}})
                {
                    if ($link->{LinkProperties} && $link->{LinkProperties}->{BatchMode})
                    {
                        $link->{BatchMode} = $link->{LinkProperties}->{BatchMode};
                    }
                    $link->{ports}  = [];
                    $link->{SrcPort}->{Name} = 'SrcPort';

                    if ($link->{SrcPort}->{NodeId} == 2 ||
                        $link->{SrcPort}->{NodeId} == 3 ||
                        $link->{SrcPort}->{NodeId} == 4)
                    {
                        $localPipeline->{sinkCount}++;
                    }

                    foreach(@{$link->{DstPort}})
                    {
                        $_->{Name} = 'DstPort';
                        if ($_->{NodeId} == 2 ||
                            $_->{NodeId} == 3 ||
                            $_->{NodeId} == 4)
                        {
                            $localPipeline->{sinkCount}++;
                        }
                        push @{$link->{ports}}, $_;
                    }

                    push @{$link->{ports}}, $link->{SrcPort};

                    if ($link->{BufferProperties})
                    {
                        $link->{buffer} = $link->{BufferProperties};
                    }

                    push @{$localPipeline->{links}}, $link;
                }
            }
            push @{$localUsecase->{pipelines}}, $localPipeline;
        }

        push @$usecases, $localUsecase;
    }

    return $usecases;
}

my ($input, $output) = @ARGV;

if (!$input || !$output)
{
    print STDERR "Usage: $0 <path to usecase XML>\n\nInvalid: `$0 $input $output`";
    exit 1;
}

if (!-e $input)
{
    print STDERR "Usage: $0 <path to usecase XML>\n\n$input not found\n";
    exit 1;
}

my $selfUsecases = GetData($input);

my $outFileObj = DiffFileHandle->new($output);
my $FILE = $outFileObj->GetHandle();

# Header
print $FILE "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef USECASEDEFS_H
#define USECASEDEFS_H

#include \"chi.h\"

#ifdef __cplusplus
extern \"C\"
{
#endif // __cplusplus

/// \@brief ranges of buffer sizes supported
struct BufferDimension
{
    UINT minWidth;
    UINT minHeight;
    UINT maxWidth;
    UINT maxHeight;
};

/// \@brief Collection of information describing how a buffer is used
struct ChiTarget
{
    ChiStreamType       direction;
    BufferDimension     dimension;
    UINT                numFormats;
    ChiBufferFormat*    pBufferFormats;
    ChiStream*          pChiStream;
};

/// \@brief Information regarding where a port interacts with a buffer directly
struct ChiTargetPortDescriptor
{
    const CHAR*           pTargetName;
    ChiTarget*            pTarget;
    ChiLinkNodeDescriptor nodeport;
};

/// \@brief List of port->buffer information
struct ChiTargetPortDescriptorInfo
{
    UINT                     numTargets;
    ChiTargetPortDescriptor* pTargetPortDesc;
};

/// \@brief Combination of pipeline information with buffer information
struct ChiPipelineTargetCreateDescriptor
{
    const CHAR*                 pPipelineName;
    ChiPipelineCreateDescriptor pipelineCreateDesc;
    ChiTargetPortDescriptorInfo sinkTarget;
    ChiTargetPortDescriptorInfo sourceTarget;
};

/// \@brief Collection of information summarizing a usecase
struct ChiUsecase
{
    const CHAR*                        pUsecaseName;
    UINT                               streamConfigMode;
    UINT                               numTargets;
    ChiTarget**                        ppChiTargets;
    UINT                               numPipelines;
    ChiPipelineTargetCreateDescriptor* pPipelineTargetCreateDesc;
};

/// \@brief Collection of usecases with matching properties (target count at this point)
struct ChiTargetUsecases
{
    UINT        numUsecases;
    ChiUsecase* pChiUsecases;
};

";
my $numUsecases = 0;
my $usecaseNames = {};
my $orderedUsecaseNames = [];

# Sort by number of targets, then by name
foreach my $usecase (sort {scalar(@{$a->{targets}}) <=> scalar(@{$b->{targets}})} sort {$a->{Name} cmp $a->{Name}} @{$selfUsecases})
{
    my $pipelines   = {};
    my $usecasename = $usecase->{Name};

    $usecaseNames->{$usecasename} = $usecase;

    push @$orderedUsecaseNames, $usecasename;

    $numUsecases++;

    print $FILE "/*==================== USECASE: $usecasename =======================*/\n\n";

    # Transform array of targets to hash for easy lookup
    my $targetMap = {};
    foreach my $target (@{$usecase->{targets}})
    {
        $targetMap->{$target->{TargetName}} = $target;

        # Generate arrays of formats to be included in the targets themselves
        print $FILE "static ChiBufferFormat $usecasename\_$target->{TargetName}\_formats[] =\n{\n";
        foreach my $format (sort @{$target->{TargetFormat}})
        {
            print $FILE "    $format,\n";
        }
        print $FILE "};\n\n";
    }

    #struct ChiTarget
    #    ChiStreamType       direction;
    #    BufferDimension     dimension;
    #    UINT                numFormats;
    #    ChiBufferFormat*    pBufferFormats;
    #    ChiStream*          pChiStream;
    my $targetArray = [];
    foreach my $target (@{$usecase->{targets}})
    {
        $target->{TargetDirection}=~s/Target/ChiStreamType/;
        print $FILE "static ChiTarget $usecasename\_$target->{TargetName}\_target =\n".
        "{\n".
        "    $target->{TargetDirection},\n".
        "    {\n".
        "        $target->{MinW},\n".
        "        $target->{MinH},\n".
        "        $target->{MaxW},\n".
        "        $target->{MaxH}\n".
        "    },\n".
        "    ".scalar(@{$target->{TargetFormat}}).",\n".
        "    $usecasename\_$target->{TargetName}\_formats,\n".
        "    NULL\n".
        "}; // $target->{TargetName}\n\n";
        push @$targetArray, "\&$usecasename\_$target->{TargetName}\_target";
    }

    print $FILE "static ChiTarget* $usecasename"."_Targets[] =\n{\n\t".join(",\n\t", @$targetArray)."\n};\n\n";
    my $pipelineNames  = {};
    my $pipelineEnums  = "enum ".$usecasename."PipelineIds\n{\n";
    my $pipelineId     = 0;

    # Essentially a DFS generating header stuff

    # Sort according to sinkCount, then by name
    foreach my $pipeline (sort {$a->{sinkCount} <=> $b->{sinkCount}} sort {$a->{Name} cmp $a->{Name}} @{$usecase->{pipelines}})
    {
        my $index = 0;
        my $pipelinename = $pipeline->{Name};
        while ($pipelineNames->{$pipelinename})
        {
            $index++;
            $pipelinename = $pipeline->{Name}."$index";
        }
        $pipelineNames->{$pipelinename} = 1;

        my $name = "$usecasename\_$pipelinename";

        $pipelineEnums .= "    ".$pipelinename." = ".$pipelineId.",\n";
        $pipelineId++;

        print $FILE "/*****************************Pipeline $pipelinename***************************/\n\n";

        my $isRT = 0;

        # parse out all ports for each node
        my $nodePorts        = {};
        my $nodeLookups      = {};
        my $linkNum          = 0;
        my $numLinkOutput    = [];
        my $sinkTargetList   = "";
        my $sourceTargetList = "";
        my $numSinkTargets   = 0;
        my $numSourceTargets = 0;
        # Use the links to determine the active nodes
        foreach my $link (@{$pipeline->{links}})
        {
            # While we're going through the nodes, setup the dest descriptors
            my $linkDest = "static ChiLinkNodeDescriptor $name"."Link$linkNum"."DestDescriptors[] =\n{\n";
            # Setup lookup for each node containing an active src or dest port

            my $isSink   = 0;
            my $isSource = 0;
            my $isBuffer = 0;
            my $targetName;

            foreach my $port (@{$link->{ports}})
            {
                $isBuffer = 1 if ($port->{NodeId} == 2 || $port->{NodeId} == 4);
                $isSink   = 1 if ($port->{NodeId} == 2 || $port->{NodeId} == 3);
                $isSource = 1 if ($port->{NodeId} == 4);
                if ($port->{NodeId} == 2 || $port->{NodeId} == 4)
                {
                    if ($targetName)
                    {
                        die "Already have target $targetName in link trying to add $port->{PortName} from $usecasename $pipelinename\n";
                    }
                    $targetName = $port->{PortName};
                }
            }

            foreach my $port (@{$link->{ports}})
            {
                $port->{isSink}   = $isSink;
                $port->{isBuffer} = $isBuffer;
                my $id  = $port->{NodeId}."_".$port->{NodeInstanceId};
                my $dir = $port->{Name};   # SrcPort or DstPort

                # Note that this is a blind trust. The name must match something in the usecase's targets or the produced header will fail compilation
                if ($targetName && ($port->{NodeId} != 2 && $port->{NodeId} != 4))
                {
                    my $nodeId     = $port->{NodeId};
                    my $nodeIn     = $port->{NodeInstanceId};
                    my $nodePort   = $port->{PortId};

                    if ($isSink)
                    {
                        $sinkTargetList .= "    {\"$targetName\", \&$usecasename\_$targetName\_target, {$nodeId, $nodeIn, $nodePort}}, // $targetName\n";
                        $numSinkTargets++;
                    }
                    elsif ($isSource)
                    {
                        $sourceTargetList .= "    {\"$targetName\", \&$usecasename\_$targetName\_target, {$nodeId, $nodeIn, $nodePort}}, // $targetName\n";
                        $numSourceTargets++;
                    }
                }

                if ($port->{NodeId} == 0)
                {
                    $isRT = 1;
                }

                # Setup a node as active using the ports used
                $nodeLookups->{$port->{NodeId}}->{$port->{NodeInstanceId}} = 1;

                if (!$nodePorts->{$id}->{$dir})
                {
                    $nodePorts->{$id}->{$dir} = [];
                }
                push @{$nodePorts->{$id}->{$dir}}, $port;

                if (($isSource == 0) && ($dir eq 'DstPort'))
                {
                    #CHILINKNODEDESCRIPTOR*  pDestNodes;            ///<  Pointer to all the dest nodes connected to the src node
                    #    UINT32                  nodeId;            ///<   Node identifier
                    #    UINT32                  nodeInstanceId;    ///<   Node instance id
                    #    UINT32                  nodePortId;        ///<   Node port id
                    #    UINT32                  portSourceTypeId;  ///<   Port source type id
                    my $portSrcTypeId = (exists $port->{PortSrcTypeId}) ? $port->{PortSrcTypeId} : "0";
                    $linkDest .= "    {$port->{NodeId}, $port->{NodeInstanceId}, $port->{PortId}, $portSrcTypeId}, // $port->{NodeName}\n";
                    $numLinkOutput->[$linkNum]++;
                }
            }
            if ($isSource == 0)
            {
                if($numLinkOutput->[$linkNum])
                {
                    print $FILE "$linkDest};\n\n";
                }
                $linkNum++;
            }
        }

        if ($sinkTargetList)
        {
            print $FILE "static ChiTargetPortDescriptor $name\_sink_TargetDescriptors[] =\n{\n$sinkTargetList};\n\n";
        }

        if ($sourceTargetList)
        {
            print $FILE "static ChiTargetPortDescriptor $name\_source_TargetDescriptors[] =\n{\n$sourceTargetList};\n\n";
        }

        # Now that we can know what ports are used by each node, iterate through the nodes
        foreach my $nodeid (sort keys(%{$nodeLookups}))
        {
            foreach my $nodein (sort keys(%{$nodeLookups->{$nodeid}}))
            {
                my $id       = $nodeid."_".$nodein;

                if ($nodeid != 2 && $nodeid != 3 && $nodeid != 4)
                {
                    if ($nodePorts->{$id}->{DstPort} && scalar(@{$nodePorts->{$id}->{DstPort}}))
                    {
                        # generate input descriptions from each port used by node
                        #CHIINPUTPORTDESCRIPTOR* pInputPorts;             ///< Pointer to input ports
                        #    UINT32                  portId;              ///<  Input/Output port id
                        #    UINT32                  isInputStreamBuffer; ///<  Does this input port take a source buffer as
                        #                                                 ///   input
                        #    UINT32                  portSourceTypeId;    ///<  Port source type id
                        print $FILE "static ChiInputPortDescriptor $name"."Node$id"."InputPortDescriptors[] =\n{\n";
                        foreach my $port (@{$nodePorts->{$id}->{DstPort}})
                        {
                            my $portSrcTypeId = (exists $port->{PortSrcTypeId}) ? $port->{PortSrcTypeId} : "0";
                            print $FILE "    {$port->{PortId}, $port->{isBuffer}, $portSrcTypeId}, // $port->{PortName}\n";
                        }
                        print $FILE "};\n\n";
                    }

                    if ($nodePorts->{$id}->{SrcPort} && scalar(@{$nodePorts->{$id}->{SrcPort}}))
                    {
                        my $pMappedSourceIds = $name."Node$id"."MappedSourceIds";
                        foreach my $port (@{$nodePorts->{$id}->{SrcPort}})
                        {
                            if (exists $port->{BypassPortSrcId})
                            {
                                print $FILE "static UINT32 $pMappedSourceIds"."[] =\n{\n  ";
                                print $FILE join(", ", @{$port->{BypassPortSrcId}})."\n";
                                print $FILE "};\n\n";
                            }
                        }
                        # generate output descriptions from each port used by node
                        #CHIOUTPUTPORTDESCRIPTOR* pOutputPorts;              ///<   Pointer to output ports
                        #    UINT32                   portId;                ///<    Input/Output port id
                        #    UINT32                   isSinkPort;            ///<    Sink port indicator
                        #    UINT32                   isOutputStreamBuffer;  ///<    Does the port output a stream buffer
                        #    UINT32                   portSourceTypeId;      ///<    Port source type id
                        #    UINT32                   numSourceIdsMapped;    ///<    Number of sources mapped to this output port for bypass
                        #    UINT32*                  pMappedSourceIds;      ///<    Source Ids mapped to this output port for bypass
                        print $FILE "static ChiOutputPortDescriptor $name"."Node$id"."OutputPortDescriptors[] =\n{\n";
                        foreach my $port (@{$nodePorts->{$id}->{SrcPort}})
                        {
                            my $portSrcTypeId = (exists $port->{PortSrcTypeId}) ? $port->{PortSrcTypeId} : "0";
                            my $numMappedSourceforBypass = (exists $port->{BypassPortSrcId}) ? scalar(@{$port->{BypassPortSrcId}}) : "0";
                            my $mappedSourceIds = (exists $port->{BypassPortSrcId}) ? $name."Node$id"."MappedSourceIds" : "NULL";
                            print $FILE "    {$port->{PortId}, $port->{isSink}, $port->{isBuffer}, $portSrcTypeId, $numMappedSourceforBypass, $mappedSourceIds}, // $port->{PortName}\n";
                        }
                        print $FILE "};\n\n";
                    }
                }
            }
        }

        #CHINODE*            pNodes;                                 ///< Pipeline nodes
        #    VOID*               pNodeProperties;                    ///<  Properties associated with the node
        #    UINT32              nodeId;                             ///<  Node identifier
        #    UINT32              nodeInstanceId;                     ///<  Node instance identifier
        #    CHINODEPORTS        nodeAllPorts;                       ///<  Information about all ports
        #        UINT32                   numInputPorts;             ///<   Number of input ports
        #        CHIINPUTPORTDESCRIPTOR*  pInputPorts;               ///<   Pointer to input ports
        #        UINT32                   numOutputPorts;            ///<   Number of output ports
        #        CHIOUTPUTPORTDESCRIPTOR* pOutputPorts;              ///<   Pointer to output ports
        my $nodeDef = "static ChiNode $name"."Nodes[] =\n{\n";
        my $numNodes = 0;
        foreach my $nodeid (sort keys(%{$nodeLookups}))
        {
            if ($nodeid != 2 && $nodeid != 3 && $nodeid != 4)
            {
                foreach my $nodein (sort keys(%{$nodeLookups->{$nodeid}}))
                {
                    my $key        = $nodeid."_".$nodein;
                    my $inCount    = $nodePorts->{$key}->{DstPort} ? scalar(@{$nodePorts->{$key}->{DstPort}}) : 0;
                    my $inName     = ($inCount) ? "$name"."Node$key"."InputPortDescriptors" : "NULL";
                    my $outCount   = $nodePorts->{$key}->{SrcPort} ? scalar(@{$nodePorts->{$key}->{SrcPort}}) : 0;
                    my $outName    = ($outCount) ? "$name"."Node$key"."OutputPortDescriptors" : "NULL";
                    my $propsName  = "$name\_node$nodeid\_$nodein\_properties";
                    my $propCount  = 0;
                    my $propString = "static ChiNodeProperty $propsName\[] =\n{\n";

                    foreach my $node (@{$pipeline->{nodes}})
                    {
                        if ($node->{NodeId} == $nodeid && $node->{NodeInstanceId} == $nodein)
                        {
                            die "More than one node matched for properties" if $propCount;

                            foreach my $prop (@{$node->{property}})
                            {
                                $propCount++;
                                $propString .= "    {$prop->{NodePropertyId}, \"$prop->{NodePropertyValue}\"},\n";
                            }
                        }
                    }

                    if ($propCount)
                    {
                        print $FILE "$propString};\n\n";
                    }
                    else
                    {
                        $propsName = "NULL";
                    }

                    $nodeDef .= "    {$propsName, $nodeid, $nodein, { $inCount, $inName, $outCount, $outName }, $propCount},\n";
                    $numNodes++;
                }
            }
        }

        print $FILE "$nodeDef};\n\n";

        #CHINODELINK*        pLinks;                       ///< Each link descriptor
        #    CHILINKNODEDESCRIPTOR   srcNode;              ///<  Src node in a link
        #        UINT32                  nodeId;           ///<   Node identifier
        #        UINT32                  nodeInstanceId;   ///<   Node instance id
        #        UINT32                  nodePortId;       ///<   Node port id
        #        UINT32                  portSourceTypeId; ///<   Port source type id
        #    UINT32                  numDestNodes;         ///<  Dest nodes in a link that the src node can be connected to
        #    CHILINKNODEDESCRIPTOR*  pDestNodes;           ///<  Pointer to all the dest nodes connected to the src node
        #        UINT32                  nodeId;           ///<   Node identifier
        #        UINT32                  nodeInstanceId;   ///<   Node instance id
        #        UINT32                  nodePortId;       ///<   Node port id
        #    CHILINKBUFFERPROPERTIES bufferProperties;     ///<  Buffer properties
        #        UINT32                  bufferFormat;     ///<   Buffer format
        #        UINT32                  bufferSize;       ///<   Buffer size (in case its a raw bytes buffer)
        #        UINT32                  bufferQueueDepth; ///<   Max buffers that will ever exist on the link
        #        UINT32                  bufferHeap;       ///<   Buffer heap
        #        UINT32                  bufferFlags;      ///<   Buffer flags
        #    CHILINKPROPERTIES       linkProperties;       ///<  Link properties
        #        UINT32                  isBatchedMode;    ///<   Batched mode indicator
        print $FILE "static ChiNodeLink $name"."Links[] =\n{\n";
        $linkNum = 0;
        foreach my $link (@{$pipeline->{links}})
        {
            my $srcPort;
            foreach my $port (@{$link->{ports}})
            {
                if ($port->{Name} eq 'SrcPort')
                {
                    $srcPort = $port;
                    last;
                }
            }
            die "Could not find srcPort for link $linkNum of $name" if (!$srcPort);

            if ($srcPort->{NodeId} != 4)
            {
                my $numDestNodes = $numLinkOutput->[$linkNum];
                my $pDestNodes   = ($numDestNodes) ? $name."Link$linkNum"."DestDescriptors" : "NULL";

                my $buffer = ($link->{buffer}) ? "{$link->{buffer}->{BufferFormat}, ".
                                                ($link->{buffer}->{BufferSize} || 0).", ".
                                                "$link->{buffer}->{BufferImmediateAllocCount}, ".
                                                "$link->{buffer}->{BufferQueueDepth}, ".
                                                "BufferHeap$link->{buffer}->{BufferHeap}, ".
                                                join("|", @{$link->{buffer}->{BufferFlags}})."}" :
                                                "{0}";

                my $portSrcTypeId = (exists $srcPort->{PortSrcTypeId}) ? $srcPort->{PortSrcTypeId} : "0";
                print $FILE "    {{$srcPort->{NodeId}, $srcPort->{NodeInstanceId}, $srcPort->{PortId}, $portSrcTypeId}, ".
                            "$numDestNodes, ".
                            "$pDestNodes, ".
                            "$buffer, ".
                            "{".(($link->{BatchMode} && $link->{BatchMode}=~/true/i) ? "1" : "0")."}},\n";

                $linkNum++;
            }
        }
        print $FILE "};\n\n";

        #typedef struct ChiPipelineTargetCreateDescriptor
        #{
        #    ChiPipleineCreateDescriptor
        #        UINT32              size;                                   ///< Size of this structure
        #        UINT32              numNodes;                               ///< Number of pipeline nodes
        #        CHINODE*            pNodes;                                 ///< Pipeline nodes
        #        UINT32              numLinks;                               ///< Number of links
        #        CHINODELINK*        pLinks;                                 ///< Each link descriptor
        #        UINT32              numTargets                              ///< Number of targets
        #        CHITARGETPORTDESCRIPTOR* pTargets;                          ///< Targets with ports
        #        UINT32              isRealTime;                             ///< Is this a realtime pipeline
        #    ChiTargetDescriptor
        #        UINT32                   TargetCount
        #        CHITARGETPORTDESCRIPTOR* pTargetDescriptors
        #}
        my $sinkTargets = ($sinkTargetList) ?     "{$numSinkTargets, $name\_sink_TargetDescriptors}" : "{0, NULL}";
        my $sourceTargets = ($sourceTargetList) ? "{$numSourceTargets, $name\_source_TargetDescriptors}" : "{0, NULL}";
        my $pipelineString = "    {\"$pipelinename\", { 0, $numNodes, $name"."Nodes, $linkNum, $name"."Links, $isRT}, $sinkTargets, $sourceTargets},  // $pipeline->{Name}\n";
        if (!$pipelines->{$pipeline->{sinkCount}})
        {
            $pipelines->{$pipeline->{sinkCount}} = [];
        }
        push @{$pipelines->{$pipeline->{sinkCount}}}, $pipelineString;
    }

    $pipelineEnums .= "};\n\n";
    print $FILE $pipelineEnums;

    print $FILE "static ChiPipelineTargetCreateDescriptor $usecasename\_pipelines[] =\n{\n";
    foreach my $sinkCount (sort {$a <=> $b} keys(%$pipelines))
    {
        print $FILE @{$pipelines->{$sinkCount}};
    }
    print $FILE "};\n\n";
}

# CHIUSECASE
#     UINT32                       streamMode                         ///< Stream mode
#     UINT32                       numTargets                         ///< Number of targets in pTargets
#     CHITARGET*                   pTargets                           ///< List of targets in the usecase
#     UINT32                       numPiplines                        ///< Number of pipelines in the usecase
#     CHIPIPELINECREATEDESCRIPTOR* pPipelines                         ///< List of pipelines in the usecase

my $targetCount = 0;
my $allUsecases = "static struct ChiTargetUsecases PerNumTargetUsecases[] =\n{";
my $usecaseEnum = "\n";

while ($numUsecases > 0)
{
    $targetCount++;

    my $usecaseStructName = "Usecases".$targetCount."Target";
    my $usecasesArray     = "static ChiUsecase ".$usecaseStructName."[] =\n{\n";
    my $count             = 0;

    foreach my $usecaseName (@$orderedUsecaseNames)
    {
        my $usecase      = $usecaseNames->{$usecaseName};
        my $numPipelines = scalar(@{$usecase->{pipelines}});
        my $numTargets   = scalar(@{$usecase->{targets}});

        if ($numTargets == $targetCount)
        {
            if ($count == 0)
            {
                $usecaseEnum .= "enum UsecaseId".$targetCount."Target\n{\n";
            }

            $usecaseEnum .= "    ".$usecaseName."Id";

            if ($count == 0)
            {
                $usecaseEnum .= "  = 0";
            }
            $usecaseEnum .= ",\n";

            $usecasesArray .= sprintf "    {%-70s, $usecase->{StreamConfigMode}, $numTargets, %-70s, $numPipelines, %-70s},\n", "\"$usecaseName\"", "$usecaseName\_Targets", "$usecaseName\_pipelines";
            $numUsecases--;
            $count++;
        }
    }

    print $FILE "$usecasesArray};\n\n" if ($count);

    $usecaseEnum .= "};\n\n" if ($count);
    $allUsecases .= ($count) ? "\t{$count, $usecaseStructName},\n" : "\t{$count, NULL},\n";
}

$allUsecases .= "};\n\n";

print $FILE "static const UINT ChiMaxNumTargets = ".$targetCount.";\n\n";
print $FILE $allUsecases;
print $FILE $usecaseEnum;

print $FILE "#ifdef __cplusplus\n".
            "}\n".
            "#endif // __cplusplus\n\n".

            "#endif // USECASEDEFS_H\n";

$outFileObj->Close();

#*******************************************************************************************************************************
#   DiffFileHandle
#
#   @brief
#       Package to manage a special filehandle that, when closed, writes the output file only if there's a change
#*******************************************************************************************************************************
package DiffFileHandle;

# After close, was the file changed?
sub WasFileChanged
{
    my ($this) = shift;

    die "Can't call WasFileChanged before Close()\n" if $this->{handle};

    return $this->{fileChange};
}

# Get the actual handle that our parent should use.
sub GetHandle
{
    my ($this) = @_;

    return $this->{handle};
}

sub GetFileName
{
    my ($this) = @_;

    return $this->{fileName};
}

sub Close
{
    my ($this) = @_;

    return if not $this->{handle};

    # If we're not writing to a memory buffer, then close the file.
    if ($this->{handleIsFile})
    {
        close $this->{handle};
        $this->{handle} = undef;
        return;
    }

    # Compare to the actual file
    close $this->{handle};   # Close the writable memory buffer
    $this->{handle} = undef;
    my ($memFH, $fileFH, $memLine);
    open($memFH, "<", \$this->{fileBuf}) or die;
    open($fileFH, "<$this->{fileName}") or die "Can't open $this->{fileName} for read\n";

    # ...Do this by comparing each line...
    while ($memLine = <$memFH>)
    {
        my $fileLine = <$fileFH>;
        if ($memLine ne $fileLine)
        {
            $this->{fileChange} = 2;
            last;
        }
    }

    close $memFH;
    close $fileFH;

    # Then write the file if necessary
    if ($this->{fileChange})
    {
        my $outFH;
        open($outFH, ">$this->{fileName}") or die "Can't open $this->{fileName} for writing\n";
        print $outFH $this->{fileBuf};
        close $outFH;
    }
}

sub new
{
    my $class = shift;

    my $this = {
                    fileName => shift,
                    handle   => undef,
                    fileBuf  => "",
                    handleIsFile => undef,
                    fileChange => 0,
               };

    bless $this, $class;

    # If file doesn't exist, life is easy
    if (not -e $this->{fileName})
    {
        $this->{handleIsFile} = 1;
        $this->{fileChange} = 1;

        # Short circuit exit. This is a new file
        open($this->{handle}, ">$this->{fileName}");
        if (!$this->{handle})
        {
            die "Can't create file: $this->{fileName}\n";
        }
        return $this;
    }

    # Make sure the file is writable
    if (not -w $this->{fileName})
    {
        die "$this->{fileName} isn't writable!\n";
    }
    if (not -r $this->{fileName})
    {
        die "$this->{fileName} isn't readable!\n";
    }

    # Open up a handle to memory
    open($this->{handle}, ">", \$this->{fileBuf}) or die;

    return $this;
}



1;
