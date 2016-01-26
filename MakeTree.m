function [Topol, CurVet, CurCon, Mmt]=MakeTree(Shem, S, Kmt)
%Function [Topology, Branches, Connections, MagnetMatrix]=MakeTree(Shema, S, Kmagnet)
%Making a tree from topology matrix
%IN:
%Shem - circuit's toplogy matrix
%S - additional resistors level
%Kmagnet - matrix of magnet connections
%OUT:
%Topology - new topology matrix
%Branches - tree's branches list
%Connections - tree's connections list
%MagnetMatrix - transformed matrix of magnet connections

Topol=Shem;
Vinxs=sortrows(horzcat((1:size(Topol,1))',Topol(:,1:2)),[2 3]);
Ri=find(Vinxs(:,2)==4);
Rlast=max(Ri); RNlast=Vinxs(Rlast,3);
Vinxs=Vinxs(:,1);
LastNode=max([max(Topol(:,3)) max(Topol(:,4))]);
Rmax=max(Topol(Vinxs(Ri,1),5))*10^S;
Rmin=min(Topol(Vinxs(Ri,1),5))*10^-S;
CurCon=[]; CurVet=[];
Ellast=size(Topol,1);
inx=1;
while inx<=Ellast
    CurVet(size(CurVet,1)+1,1)=Vinxs(inx);
    CurVet(size(CurVet,1),2:3)=Topol(Vinxs(inx),3:4);
    if havecycle(CurVet(:,2:3))
        if Topol(Vinxs(inx),1)<4
            %we have cycle and element is E,C,VSopen
            LastNode=LastNode+1; RNlast=RNlast+1;
            Ellast=Ellast+1;
            Topol(Ellast,:)=[4 RNlast LastNode Topol(Vinxs(inx),4) Rmin 0];
            Topol(Vinxs(inx),4)=LastNode;
            Vinxs=vertcat(Vinxs(1:Rlast),Ellast,Vinxs((Rlast+1):(Ellast-1)));
            Rlast=Rlast+1;
            CurCon=[]; CurVet=[]; inx=0;
        else
            CurCon(size(CurCon,1)+1,:)=CurVet(size(CurVet,1),:);
            CurVet(size(CurVet,1),:)=[];
        end
    else
        if Topol(Vinxs(inx),1)>4
            %we have no cycle and element is J,L,VSclose
            Ellast=Ellast+1; RNlast=RNlast+1;
            Topol(Ellast,:)=[4 RNlast Topol(Vinxs(inx),4) Topol(Vinxs(inx),3) Rmax 0];
            Vinxs=vertcat(Vinxs(1:Rlast),Ellast,Vinxs((Rlast+1):(Ellast-1)));
            CurCon=[]; CurVet=[]; inx=0;
            Rlast=Rlast+1;
        end
    end
    inx=inx+1;
end            
% Here we transform magnetic connections' matrix
try
    if ~isempty(Kmt)
        for n=1:size(Kmt,1)
            L1_inx=find(Topol(:,1)==6 & Topol(:,2)==Kmt(n,1));
            L2_inx=find(Topol(:,1)==6 & Topol(:,2)==Kmt(n,2));
            M=Kmt(n,3)*sqrt(Topol(L1_inx,5)*Topol(L2_inx,5));
            Mmt(n,:)=[L1_inx L2_inx M];
        end
    else
        Mmt=[];
    end
catch
    error('Error in MakeTree.m: Unable to transform magnetic connections matrix')
end