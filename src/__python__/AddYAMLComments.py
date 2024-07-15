# This Python file uses the following encoding: utf-8

from ruamel.yaml import YAML
import ruamel.yaml.comments
import ruamel.yaml.scalarstring
import sys

comment_file_dict = {
    "SC_": "SC_comments.yaml",
    "Orb_": "Orb_comments.yaml",
    "Nodes_": "Nodes_comments.yaml",
    "Inp_DSM": "InpDSM_comments.yaml",
    "Inp_FOV": "InpFOV_comments.yaml",
    "Inp_Graphics": "InpGraphics_comments.yaml",
    "Inp_IPC": "InpIPC_comments.yaml",
    "Inp_NOS3": "InpNOS3_comments.yaml",
    "Inp_Region": "InpRegion_comments.yaml",
    "Inp_Sim": "InpSim_comments.yaml",
    "Inp_TDRS": "InpTDRS_comments.yaml",
}
startswith_type = {
    "SC_",
    "Orb_",
    "Nodes_",
}


def convertToYamlAndComment(data, commentDict):
    data = convertToYamlObjects(data)
    if commentDict is not None:
        for key in commentDict.keys():
            data.yaml_set_comment_before_after_key(key, after=commentDict[key])
        data.yaml_set_start_comment(commentDict["SOF"])
    return data


def convertToYamlObjects(data):
    if data:
        match data:
            case dict():
                for key in data.keys():
                    data[key] = convertToYamlObjects(data[key])
                data = ruamel.yaml.comments.CommentedMap(data)
            case list():
                if isinstance(data[0], (list, dict)):
                    for ind in range(len(data)):
                        data[ind] = convertToYamlObjects(data[ind])
                data = ruamel.yaml.comments.CommentedSeq(data)
    return data


if __name__ == "__main__":
    app_path = sys.argv[1]
    inout_dir = sys.argv[2]
    file = sys.argv[3]

    inout_dir += "/"

    yaml = YAML()
    yaml.version = (1, 2)
    yaml.indent(
        mapping=2,
        sequence=4,
        offset=2,
    )
    yaml.preserve_quotes = True

    yaml_comment_dir = app_path + "/__python__/yaml_comments/"
    f_types = comment_file_dict.keys()
    for f_type in comment_file_dict:
        if file.startswith(f_type):
            yaml_comment_file = comment_file_dict[f_type]

    with open(yaml_comment_dir + yaml_comment_file) as f:
        yaml_comments = yaml.load(f)
    with open(inout_dir + file) as f:
        yaml_data = yaml.load(f)
    yaml_data = convertToYamlAndComment(yaml_data, yaml_comments)
    with open(inout_dir + file, "w") as f:
        yaml.dump(yaml_data, f)
