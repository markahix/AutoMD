#include "python.h"

void python::plot_csv_data(std::string filename)
{
    std::string pyscript = R"PLOTCSVDATA(
#!/usr/bin/env python3
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.ndimage import gaussian_filter
import pandas as pd
import sys
input_csv=sys.argv[1]
x_label_fontsize=10
y_label_fontsize=10
tick_fontsize=9
df = pd.read_csv(input_csv,comment="#")
x_values = df["TIME(PS)"]/1000
xmin = min(x_values)
xmax = max(x_values)
fig = plt.figure(figsize=(8,8),dpi=300)
y_data     = df["TEMP(K)"]
y_label    = "Temperature (K)"
plot_color ="red"
ax = fig.add_subplot(4,1,1)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["PRESS"]
y_label    = "Pressure (bar)"
plot_color = "blue"
ax = fig.add_subplot(4,1,2)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["VOLUME"]
y_label    = r"Volume ($\AA^3$)"
plot_color = "green"
ax = fig.add_subplot(4,1,3)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["Density"]
y_label    = r"Density ($g\cdot mL^{-1}$)"
plot_color = "orange"
ax = fig.add_subplot(4,1,4)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xlabel("Time (ns)",fontsize=x_label_fontsize)
fig.subplots_adjust(hspace=0.0)
fig.savefig("00_Report/"+input_csv.replace(".csv","_Figure_01.png"),dpi=300,facecolor="white")
fig = plt.figure(figsize=(8,6),dpi=300)
y_data     = df["Etot"]
y_label    = "Total Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color ="purple"
ax = fig.add_subplot(3,1,1)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["EKtot"]
y_label    = "Kinetic Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color = "blue"
ax = fig.add_subplot(3,1,2)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["EPtot"]
y_label    = "Potential Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color = "red"
ax = fig.add_subplot(3,1,3)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xlim(xmin,xmax)
ax.set_xlabel("Time (ns)",fontsize=x_label_fontsize)
fig.subplots_adjust(hspace=0.0)
fig.savefig("00_Report/"+input_csv.replace(".csv","_Figure_02.png"),dpi=300,facecolor="white")
fig = plt.figure(figsize=(8,10),dpi=300)
y_data     = df["EKtot"]
y_label    = "Bond Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color = "blue"
ax = fig.add_subplot(5,1,1)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["ANGLE"]
y_label    = "Angle Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color = "red"
ax = fig.add_subplot(5,1,2)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["DIHED"]
y_label    = "Dihedral Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color = "green"
ax = fig.add_subplot(5,1,3)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["VDWAALS"]
y_label    = "van der Waals Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color = "orange"
ax = fig.add_subplot(5,1,4)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xticks([])
y_data     = df["EELEC"]
y_label    = "Electrostatic Energy \n"+r"($kcal\cdot mol^{-1}$)"
plot_color = "black"
ax = fig.add_subplot(5,1,5)
ax.plot(x_values,y_data,lw=0.5,color="grey",alpha=0.5,linestyle="-")
ax.plot(x_values,gaussian_filter(y_data,sigma=100),color=plot_color,alpha=1.0,lw=1.0)
ax.set_ylabel(y_label,fontsize=y_label_fontsize)
ax.tick_params(axis="both", labelsize=tick_fontsize)
ax.set_xlim(xmin,xmax)
ax.set_xlabel("Time (ns)",fontsize=x_label_fontsize)
fig.subplots_adjust(hspace=0.0)
fig.savefig("00_Report/"+input_csv.replace(".csv","_Figure_03.png"),dpi=300,facecolor="white")
)PLOTCSVDATA";
    utils::write_to_file("plotcsvdata.py",pyscript);
    std::stringstream buffer;
    buffer.str("");
    buffer << "python plotcsvdata.py ";
    buffer << filename <<"; rm plotcsvdata.py";
    utils::silent_shell(buffer.str().c_str());
}

void python::plot_cpptraj_results()
{
    std::string pyscript = R"CPPTRAJ(
#!/usr/bin/env python3
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.ndimage import gaussian_filter
import numpy as np
import pandas as pd
from glob import glob as G
data = np.genfromtxt("nma_prot_first_100_modes.nmd",delimiter=None,skip_header=9)
area = np.pi * (2)**2
x = data[0,2] * data[0,3:]
y = data[1,2] * data[1,3:]
z = -(-x**2 - y**2)
fig = plt.figure(figsize=(6,6))
ax = fig.add_subplot(1,1,1)
ax.set_xlabel("Mode 1",fontsize=16)
ax.set_ylabel("Mode 2",fontsize=16)
ax.scatter(x,y,marker='o', s=area, zorder=10, alpha=0.4, c=z, edgecolors = 'black', cmap='viridis')
plt.xticks(fontsize=14,rotation=90)
plt.yticks(fontsize=14)
plt.tight_layout()
fig.savefig("00_Report/PrincipleComponentAnalysis.png",dpi=300,facecolor="white")
fig = plt.figure(figsize=(7,6),dpi=300)
data = np.genfromtxt("correlation.dat")
dims = data.shape[0]
X, Y = np.mgrid[0:dims:complex(0, dims), 0:dims:complex(0, dims)]
ax = fig.add_subplot(1,1,1)
im = ax.pcolormesh(X,Y,data,cmap="viridis",vmin=-1.,vmax=1.)
fig.colorbar(im,ticks=[-1,0,1],ax=ax)
ax.xaxis.tick_bottom()
plt.xticks(rotation=90)
fig.savefig("00_Report/Correlation.png",dpi=300,facecolor="white")
data = np.genfromtxt("RMSD.dat",skip_header=1,usecols=1)
x = np.arange(0,len(data),dtype=float)/1000
smoothed = gaussian_filter(data,sigma=100)
fig = plt.figure(figsize=(6,4),dpi=300)
ax = fig.add_subplot(1,1,1)
ax.set_xlabel("Time (ns)")
ax.set_ylabel(r"RMSD ($\AA$)")
ax.set_xlim(0,x.max())
ax.plot(x,data,color="grey",alpha=0.5)
ax.plot(x,smoothed,color="blue",alpha=1.0)
fig.savefig("00_Report/RMSD.png",dpi=300,facecolor="white")
data = np.genfromtxt("RMSF_ByRes.dat",skip_header=1,usecols=1)
fig = plt.figure(figsize=(6,4),dpi=300)
ax = fig.add_subplot(1,1,1)
ax.set_xlim(0,len(data)+1)
ax.bar(np.arange(1,len(data)+1,1),data,align="center")
fig.savefig("00_Report/RMSF_ByRes.png",dpi=300,facecolor="white")
if G("LigInterEnergy.dat"):
    data = np.genfromtxt("LigInterEnergy.dat",usecols=(1,2),skip_header=1)
    coulomb = data[:,0]
    vdw = data[:,1]
    fig = plt.figure(figsize=(6,8),dpi=300)
    ax = fig.add_subplot(2,1,1)
    x = np.arange(0,len(coulomb),dtype=float)/1000
    smoothed = gaussian_filter(coulomb,sigma=100)
    ax.set_xlabel("Time (ns)")
    ax.set_ylabel(r"Coulomb Energy ($kcal\cdot mol^{-1}$)")
    ax.set_xlim(0,x.max())
    ax.plot(x,coulomb,color="grey",alpha=0.5)
    ax.plot(x,smoothed,color="blue",alpha=1.0)
    ax = fig.add_subplot(2,1,2)
    x = np.arange(0,len(vdw),dtype=float)/1000
    smoothed = gaussian_filter(vdw,sigma=100)
    ax.set_xlabel("Time (ns)")
    ax.set_ylabel(r"van der Waals Energy ($kcal\cdot mol^{-1}$)")
    ax.set_xlim(0,x.max())
    ax.plot(x,coulomb,color="grey",alpha=0.5)
    ax.plot(x,smoothed,color="blue",alpha=1.0)
    fig.savefig("00_Report/Ligand_Interaction_Energy.png",dpi=300,facecolor="white")
if G("SASA.dat"):
    df = pd.read_csv("SASA.dat",delim_whitespace=True)
    df["Time (ns)"] = np.arange(0,len(df["#Frame"]),dtype=float)/1000
    df = df[df["Receptor"] != -1]
    df = df[df["Ligand"] != -1]
    df = df[df["Complex"] != -1]
    df["Interface"] = df["Receptor"] + df["Ligand"] - df["Complex"]
    fig = plt.figure(figsize=(7.5,10),dpi=300)
    ax = fig.add_subplot(4,1,1)
    data = df["Receptor"]
    smoothed = gaussian_filter(data,sigma=100)
    ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
    ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
    ax.set_ylabel(r"Receptor SASA ($\AA^2$)")
    ax.set_xticks([])
    ax = fig.add_subplot(4,1,2)
    data = df["Ligand"]
    smoothed = gaussian_filter(data,sigma=100)
    ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
    ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
    ax.set_ylabel(r"Ligand SASA ($\AA^2$)")
    ax.set_xticks([])
    ax = fig.add_subplot(4,1,3)
    data = df["Complex"]
    smoothed = gaussian_filter(data,sigma=100)
    ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
    ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
    ax.set_ylabel(r"Complex SASA ($\AA^2$)")
    ax.set_xticks([])
    ax = fig.add_subplot(4,1,4)
    data = df["Interface"]
    smoothed = gaussian_filter(data,sigma=100)
    ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
    ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
    ax.set_ylabel(r"Buried Interface ($\AA^2$)")
    ax.set_xlabel("Time (ns)")
    ax.set_xlim(0,max(df["Time (ns)"]))
    fig.subplots_adjust(hspace=0.0)
    fig.savefig("00_Report/SolventAccessibleSurfaceArea.png",dpi=300,facecolor="white")
)CPPTRAJ";
    utils::write_to_file("plotcpptrajdata.py",pyscript);
    std::stringstream buffer;
    buffer.str("");
    buffer << "python plotcpptrajdata.py; rm plotcpptrajdata.py ";
    utils::silent_shell(buffer.str().c_str());
    return;
}