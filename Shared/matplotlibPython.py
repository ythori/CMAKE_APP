import numpy as np
import matplotlib.pyplot as plt

# ==============================================================
# input args: args["dataset", "params", "interface"]
#   dataset:   data_dic["figure[n]"] = { "data[i]" : data_arr }
#   params:    params_dic["figure[n]"] = { "title", "legend", "xy_label", "type", "xy_ticks", "xy_labels", "xy_lim_range",
#                                          "is_axis", "color", "n_color" ... }
#                   keys: If exist, set as matplotlib params, if not exist, remain default.
#                   { "title" : string }
#                   { "legend" : 1 }
#                   { "xy_label" : string }
#                   { "type" : [data_type_list] }   ex) [ "line", "line" ]
#                   { "xy_ticks" : [tick_list] }            (Not implemented)
#                   { "xy_labels" : [tick_label_list] }     (Not implemented)
#                   { "xy_lim_range" : [low_double, high_double] } (Not implemented)
#                   { "is_Axis" : 1 }
#                   { "color" : "jet" or "orange"}
#                   { "n_color" : int }
#
#   interface: interface_dic["show", "save", "path"]
#                   # interface_dic["show"]: 0 or 1
#                   # interface_dic["save"]: 0 or 1
#                   # interface_dic["path"]: "NULL" or save_path
# ==============================================================


def matplotlibPlot(args_dic):
    data_dic = args_dic["dataset"]
    params_dic = args_dic["params"]
    interface_dic = args_dic["interface"]
    #print(data_dic)
    #print(params_dic)
    #print(interface_dic)

    fig = plt.figure()
    n_figure = len(data_dic)
    
    for n in range(n_figure):
        fig_key = "figure" + str(n + 1)     # "figure1", "figure2", ...
        print(fig_key)
        one_figure_plot_data = data_dic[fig_key]
        one_figure_params = params_dic[fig_key]
        ax = fig.add_subplot(n_figure, 1, n + 1)
        #print(one_figure_plot_data)
        #print(one_figure_params)
        
        is_color = False
        is_label = False

        if "is_axis" in one_figure_params:
            ax.tick_params(labelbottom=False, labelleft=False, labelright=False, labeltop=False)
            ax.tick_params(bottom=False, left=False, right=False, top=False)

        if "x_lim_range" in one_figure_params:
            lim_range = one_figure_params["x_lim_range"]    # list[min, max]
            ax.set_xlim(lim_range)

        if (("color" in one_figure_params) and ("n_color" in one_figure_params)):
            is_color = True
            color = one_figure_params["color"]
            n_color = one_figure_params["n_color"]
            #print(color, n_color)
            separate_num = int(len(one_figure_plot_data) / n_color) + 1
            color_map_div = plt.get_cmap(color, separate_num)

        #print(one_figure_params)
        if "title" in one_figure_params:    # TODO
            ax.set_title(one_figure_params["title"])

        if "xlabel" in one_figure_params:
            ax.set_xlabel(one_figure_params["xlabel"])

        if "ylabel" in one_figure_params:
            ax.set_ylabel(one_figure_params["ylabel"])

        if "is_legend" in one_figure_params:
            is_legend = one_figure_params["is_legend"]

        for i in range(len(one_figure_plot_data)):
            data_key = "data" + str(i + 1)  # "data1", "data2", ...
            data_arr = one_figure_plot_data[data_key]
            plot_type = one_figure_params["type"][i]    # line, scatter, heatmap
            style = one_figure_params["style"][i]
            label = one_figure_params["legend"][i]
            color = ""
            linestyle = ""
            if len(style) > 1:
                if style[0] == "r":
                    color = "red"
                elif style[0] == "b":
                    color = "blue"
                elif style[0] == "k":
                    color = "black"
                elif style[0] == "g":
                    color = "green"
                elif style[0] == "y":
                    color = "yellow"
                linestyle = style[1:]

            if plot_type == "line_val_only":
                if len(style) > 1:
                    ax.plot(data_arr, label=label, color=color, linestyle=linestyle)
                else:
                    ax.plot(data_arr, label=label)

            if plot_type == "line_with_pos":
                #print("line")
                #print(data_arr)
                if is_color is True:
                    #print(i)
                    color_num = int(i / n_color)
                    #print(color_num)
                    assigned_color = color_map_div(color_num)
                    ax.plot(data_arr[:, 0], data_arr[:, 1], color=assigned_color)   # data: arr([start_pos], [end_pos])
                else:
                    if len(style) > 1:
                        ax.plot(data_arr[:, 0], data_arr[:, 1], label=label, color=color, linestyle=linestyle)
                    else:
                        ax.plot(data_arr[:, 0], data_arr[:, 1], label=label)

            elif plot_type == "scatter":
                ax.scatter(data_arr)

            elif plot_type == "heatmap":
                #print("heat")
                #print(data_arr)
                color_map = plt.cm.jet
                if "color" in one_figure_params:
                    if one_figure_params["color"] == "Oranges":
                        color_map = plt.cm.Oranges

                ax.pcolor(data_arr.T, cmap=color_map)

        if is_legend == 1:
            #print("trueeee")
            ax.legend()



    if interface_dic["save"] == 1:      # Absolutely executed savefig() before show()
        print("save_true", interface_dic["path"])
        plt.savefig(interface_dic["path"])

    if interface_dic["show"] == 1:
        print("show_true")
        plt.show()




#def matplotlibDTWPlot(args_dic):
#    data_dic = args_dic["dataset"]  # data1, path, data2
#    params_dic = args_dic["params"]
#    interface_dic = args_dic["interface"]

#    data1 = data_dic["data1"]
#    path = data_dic["data2"]
#    data2 = data_dic["data3"]

#    fig, axs = plt.subplots(3, 1)
#    for i in range(3):
#        axs[i].tick_params(labelbottom=False,
#                           labelleft=False,
#                           labelright=False,
#                           labeltop=False)
#        axs[i].tick_params(bottom=False,
#                           left=False,
#                           right=False,
#                           top=False)

#    heatmap_data1 = axs[0].pcolor(data1.T, cmap=plt.cm.Oranges) # data:[adata, dim] -> [dim, adata]
#    mod_rate = float(len(data1))/len(data2)  # ex) 18 / 16
#    n_color_div = 20
#    separate_num = int(len(data1) / n_color_div) + 1
#    color_map = plt.get_cmap("jet", separate_num)
#    #print(mod_rate)
#    for i, p in enumerate(path):
#        pos_data1 = p[0]                # ex) end: 18
#        pos_data2 = p[1] * mod_rate     # ex) end: 16 * 18 / 16
#        color_num = int(i / n_color_div)
#        assigned_color = color_map(color_num)
#        #print(pos_data2+0.5, pos_data1+0.5)
#        if i%3 == 0:
#            axs[1].plot((pos_data2+0.5, pos_data1+0.5), (0, 1), color=assigned_color)     # (under, top), subplot_top: data1, subplot_under: data2
#    axs[1].set_xlim([0, len(data1)])
#    #axs[0].invert_yaxis()
#    heatmap_data2 = axs[2].pcolor(data2.T, cmap=plt.cm.Oranges)

#    if interface_dic["save"] == 1:      # Absolutely executed savefig() before show()
#        print("save_true", interface_dic["path"])
#        plt.savefig(interface_dic["path"])

#    if interface_dic["show"] == 1:
#        print("show_true")
#        plt.show()


#class MatplotlibCpp:
#    data_info_list = None
#    save_flag = None
#    save_folder = None
#    save_file_name = None

#    def __init__(self):
#        print(12345)
#        self.data_info_list = []
#        self.save_flag = False
#        self.save_folder = ""
#        self.save_file_name = ""

#    def execute(self, *args):
#        print("exexe")
#        print(args)
#        show_flag = args[0]
#        print("flag", show_flag)
#        fig = plt.figure()
#        print(113355)
#        print(self.save_folder)
#        print(self.save_file_name)
#        for i, data_info in enumerate(self.data_info_list):
#            ax = fig.add_subplot(len(self.data_info_list), 1, i + 1)

#            # Set data
#            if data_info["plot_type"] == "line":
#                for data in data_info["data"]:
#                    print(data)
#                    ax.plot(data)
#            elif data_info["plot_type"] == "heatmap":
#                data = np.array(data_info["data"][0])
#                heatmap = ax.pcolor(data.T, cmap=plt.cm.Oranges)

#            # Set parameter



#        if show_flag == 1:
#            print("show_true")
#            plt.show()
#        if self.save_flag is True:
#            print("save_true")
#            plt.savefig(save_folder + save_file_name)

#    def getIdx(self, idx):
#        if idx == -1:
#            return -1
#        else:
#            return min(len(self.data_info_list)-1, idx)

#    def addData(self, *args):
#        act_idx = getidx(idx)
#        data = args[0][0]
#        att = args[0][1]
#        idx = args[0][2]
#        self.data_info_list[act_idx]["data"].append(data)
#        self.data_info_list[act_idx]["plot_type"] = att

#    def setData(self, *args):
#        #print("settttt")
#        data = args[0][0]
#        att = args[0][1]
#        print(data, att)
#        data_dic = {"data": [data], "title": None, "legend": None, "plot_type": att,
#                    "x": {"ticks": [], "labels": [], "label": None, "lim_range": []},
#                    "y": {"ticks": [], "labels": [], "label": None, "lim_range": []}}
#        self.data_info_list.append(data_dic)

#    def setSaveFolder(self, *args):
#        print("folder", args[0])
#        self.save_folder = args[0]
#        self.save_flag = True

#    def setSaveFileName(self, *args):
#        print("save", args[0])
#        print(self.save_folder)
#        self.save_file_name = args[0]





#def mattest():
#    toy = []
#    print("mattest")
#    for i in range(50):
#        toy.append(i + 0.5);
#    print(toy)
#    plt.plot(toy)
#    print(111)
#    plt.show()



#if __name__ == "__main__":
#    print(111)
#    PLT = MatplotlibCpp()
#    save_folder = "C:/Users/brume/Documents/Project/DB/Research/Results/test_matplotlib_write/"
#    save_file_name = "test2.png"
#    show_flag = True
#    toy = []
#    for i in range(50):
#        toy.append(i + 0.5);
#    PLT.setData(toy)
#    PLT.setSaveFolder(save_folder)
#    PLT.setSaveFileName(save_file_name)
#    PLT.execute(show_flag)

   


