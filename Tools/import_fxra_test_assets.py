import unreal

ROOT = "G:\\" + "UE5\u91cd\u5b9a\u5411\u63d2\u4ef6\u5f00\u53d1"
MIXAMO_DIR = ROOT + "\\" + "Mixamo\u52a8\u753b"

IMPORTS = [
    {
        "filename": ROOT + "\\SKM_Manny_Simple.FBX",
        "destination": "/Game/FXRA_Imported/UE5Manny",
        "import_mesh": True,
        "import_animations": False,
    },
    {
        "filename": ROOT + "\\SKM_MHC_Default_BodyMesh.FBX",
        "destination": "/Game/FXRA_Imported/MetaHuman",
        "import_mesh": True,
        "import_animations": False,
    },
    {
        "filename": MIXAMO_DIR + "\\Center Block.fbx",
        "destination": "/Game/FXRA_Imported/Mixamo",
        "import_mesh": True,
        "import_animations": True,
    },
    {
        "filename": MIXAMO_DIR + "\\Hip Hop Dancing.fbx",
        "destination": "/Game/FXRA_Imported/Mixamo",
        "import_mesh": True,
        "import_animations": True,
    },
    {
        "filename": MIXAMO_DIR + "\\Swing Dancing.fbx",
        "destination": "/Game/FXRA_Imported/Mixamo",
        "import_mesh": True,
        "import_animations": True,
    },
]


def set_if_available(obj, name, value):
    try:
        obj.set_editor_property(name, value)
        return True
    except Exception:
        return False


def make_fbx_options(import_mesh, import_animations):
    options = unreal.FbxImportUI()
    options.set_editor_property("import_mesh", import_mesh)
    options.set_editor_property("import_animations", import_animations)
    options.set_editor_property("import_as_skeletal", True)
    options.set_editor_property("import_materials", False)
    options.set_editor_property("import_textures", False)
    options.set_editor_property("create_physics_asset", False)

    skeletal_options = options.get_editor_property("skeletal_mesh_import_data")
    if skeletal_options:
        set_if_available(skeletal_options, "update_skeleton_reference_pose", False)

    anim_options = options.get_editor_property("anim_sequence_import_data")
    if anim_options:
        set_if_available(anim_options, "import_bone_tracks", True)

    return options


def make_task(item):
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", item["filename"])
    task.set_editor_property("destination_path", item["destination"])
    task.set_editor_property("automated", True)
    task.set_editor_property("save", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("options", make_fbx_options(item["import_mesh"], item["import_animations"]))
    return task


def main():
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    tasks = [make_task(item) for item in IMPORTS]
    asset_tools.import_asset_tasks(tasks)

    for task in tasks:
        unreal.log("FXRA import: {} -> {}".format(task.filename, task.imported_object_paths))

    unreal.EditorAssetLibrary.save_directory("/Game/FXRA_Imported", only_if_is_dirty=False, recursive=True)


if __name__ == "__main__":
    main()
