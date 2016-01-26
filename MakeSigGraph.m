function SigGraph=MakeSigGraph(Topo, Branch, Connect, Mmt)
%Function SigGraph=MakeSigGraph(Topology, Branches, Connections, MagnetMatrix)
%Form signal graph
%IN:
%Topology - circuit topology matrix
%Branches - circuit tree branches list
%Connections - circuit tree connections list
%MagnetMatrix - matrix of magnetic connections
%OUT:
%SigGraph - signal graph

SigGraph=[];
for k=1:size(Branch,1)
    inx=Branch(k,1);
  if Topo(inx,1)~=1
    SigGraph(1:2,size(SigGraph,2)+1)=[3*inx-1; 3*inx];
    switch Topo(inx,1)
    case 2
        SigGraph(3:4,size(SigGraph,2))=[1/Topo(inx,5); 1];
        SigGraph(:,size(SigGraph,2)+1)=[3*inx-2; 3*inx; 1; 0];
    case 3
        SigGraph(3:4,size(SigGraph,2))=[0; 0];
    case 4
        SigGraph(3:4,size(SigGraph,2))=[Topo(inx,5); 0];
    end
  end
end
for k=1:size(Connect,1)
    inx=Connect(k,1);
  if Topo(inx,1)~=7
    SigGraph(1:2,size(SigGraph,2)+1)=[3*inx; 3*inx-1];
    switch Topo(inx,1)
    case 4
        SigGraph(3:4,size(SigGraph,2))=[1/Topo(inx,5); 0];
    case 5
        SigGraph(3:4,size(SigGraph,2))=[0; 0];
    case 6
        SigGraph(3:4,size(SigGraph,2))=[1/Topo(inx,5); 1];
        SigGraph(:,size(SigGraph,2)+1)=[3*inx-2; 3*inx-1; 1; 0];
    end
  end
    Cycle=MarkCycle(Branch, Connect(k,:));
    for m=1:size(Cycle,1)
        inxb=Cycle(m,1);
        SigGraph(:,size(SigGraph,2)+1)=[3*inxb; 3*inx; -Cycle(m,2); 0];
        SigGraph(:,size(SigGraph,2)+1)=[3*inx-1; 3*inxb-1; Cycle(m,2); 0];
    end
end
if ~isempty(Mmt)
    for k=1:size(Mmt,1)
        SigGraph(:,size(SigGraph,2)+1)=[3*Mmt(k,1)-1; 3*Mmt(k,2)-1; -Mmt(k,3)/Topo(Mmt(k,2),5); 0];
        SigGraph(:,size(SigGraph,2)+1)=[3*Mmt(k,1)-2; 3*Mmt(k,2)-1; Mmt(k,3)/Topo(Mmt(k,2),5); 0];
        SigGraph(:,size(SigGraph,2)+1)=[3*Mmt(k,2)-1; 3*Mmt(k,1)-1; -Mmt(k,3)/Topo(Mmt(k,1),5); 0];
        SigGraph(:,size(SigGraph,2)+1)=[3*Mmt(k,2)-2; 3*Mmt(k,1)-1; Mmt(k,3)/Topo(Mmt(k,1),5); 0];
    end
end