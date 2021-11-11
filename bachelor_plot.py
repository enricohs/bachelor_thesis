import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data_folder = "C:\\Users\\Enrico\\Desktop\\Enrico\\Uni\\Letztes Semester\\Bachelor\\data\\"


"""
Creates Plots for one experiment.

:param ExpName: Experiment Name: Plots the Data for this experiment
:param plot_title: Title of the plots; Experiment X
"""
def plot_experiment(ExpName, plot_title):
    df_hx = pd.read_csv(data_folder + ExpName + "-hx.csv", delimiter="\t")
    df_imu = pd.read_csv(data_folder + ExpName + "-imu.csv", delimiter="\t")

    # hx
    hx_slice = df_hx[["grid weight"]]
    hx_slice2 = df_hx[["posX*", "posY*"]]
    #imu_slice = df_imu[]
    ax = hx_slice.plot()
    ax.set(xlabel="time-steps", ylabel="gram [g]", title=plot_title)

    ax1 = hx_slice2.plot()
    ax1.set(xlabel="time-steps", ylabel="Position [cm]", title=plot_title)
    fig = ax.get_figure()
    plt.show()
    #fig.savefig(data_folder + ExpName + "d", bbox_inches='tight')


plot_experiment("exp2-2", "Experiment 2.2")