#include <iostream>
#include <format>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "widgets/imfiledialog/ImFileDialog.h"
#include "widgets/imguicolortextedit/TextEditor.h"

// #include "widgets/imguinodeeditor/imgui_node_editor.h"
#include "widgets/imnodes/imnodes.h"

#include "rapidjson/document.h"
#include "rapidjson/schema.h"

struct NodeLink {
    int id;
    int start_attr, end_attr;
};

int main()
{
    const auto testJson = R"(
    {
        "hello": "world",
        "t": true,
        "f": false,
        "n": null,
        "a":[1, 2, 3, 4, 5],
        "long-string": "long\nlooooooong\nmaaaan~!\n"
    }
    )";

    const auto schemaJson = R"(
    {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "type": "object",
        "properties":
        {
            "hello":
            {
                "type": "string"
            },
            "a":
            {
                "type": "array",
                "items":
                {
                   "type": "number"
                },
                "minItems": 2,
                "maxItems": 3
            }
        }
    }
    )";

    rapidjson::Document schemaDoc1;
    schemaDoc1.Parse(schemaJson);

    rapidjson::SchemaDocument schemaDoc(schemaDoc1);
    rapidjson::SchemaValidator jsonValidator(schemaDoc);

    rapidjson::Document doc;
    doc.Parse(testJson);

    if (!doc.Accept(jsonValidator))
    {
        rapidjson::StringBuffer sb;

        jsonValidator.GetInvalidSchemaPointer().StringifyUriFragment(sb);

        std::cerr << std::format("Invalid schema: {}\n", sb.GetString());

        sb.Clear();

        jsonValidator.GetInvalidDocumentPointer().StringifyUriFragment(sb);

        std::cerr << std::format("Invalid document: {}\n", sb.GetString());
    }

    std::cout << std::format("Original JSON:\n {}\n", testJson);

    auto s = doc["long-string"].GetString();

    std::cout << std::format("long string: '{}'\n", s);

    return 0;
}

int main1(void)
{
    GLFWwindow* window;

    if (!glfwInit())
    {
        return -1;
    }

    const char* glsl_version = "#version 460";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only

    auto monitor = glfwGetPrimaryMonitor();
    // int width_mm, height_mm;
    // glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);
    float scale_x, scale_y;
    glfwGetMonitorContentScale(monitor, &scale_x, &scale_y);

    std::cout << std::format("Monitor scale: {} x {}\n", scale_x, scale_y);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024 * scale_x, 768 * scale_y, "Hello World", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    auto font = io.Fonts->AddFontFromFileTTF("resources/fonts/NotoSans-Light.ttf", 28);

    io.Fonts->Build();

    // io.FontGlobalScale = scale_y;

    auto style = &ImGui::GetStyle();
    style->ScaleAllSizes(scale_y);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;

    ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
        // glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return (void*) tex;
    };

    ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
        GLuint texID = (GLuint) tex;
        glDeleteTextures(1, &texID);
    };

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;

    //ax::NodeEditor::Config config;
    //// config.SettingsFile = "Simple.json";
    //ax::NodeEditor::EditorContext* m_Context = ax::NodeEditor::CreateEditor(&config);

    ImNodes::CreateContext();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        io.FontDefault = font;

        ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("Tools", nullptr, window_flags);
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

        if (ImGui::BeginTabBar("MainTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Getting started"))
            {
                ImGui::TextWrapped(
                    "The workflow looks like this:\n"
                    "1. Import assets (3D models, sounds, images/sprites)\n"
                    "2. Configure shaders for each asset - these will define how assets are rendered; the default shaders are provided upon import to get you going\n"
                    "3. Place models on a scene in 3D space\n"
                    "4. Define signals and their handlers - a signal could be anything - from a key pressed, timer ticking all the way to a collision between two objects on a scene or scene transition\n"
                    "5. Set up rendering pipeline - the default pipeline does not have any post-processing effects, just straight rendering of an entire scene. On this step you can implement deferred"
                    "rendering techniques (such as screen-space ambient occlusion, shadow mapping, etc.) and rendering optimizations (like batch rendering geometry)\n"
                );

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Assets & shaders"))
            {
                ImGui::Text("Import and configure assets here, define shaders for each asset");

                if (ImGui::Button("Import"))
                {
                    ifd::FileDialog::Instance().Open("open_assets", "Import assets", "*.*");
                }

                if (ifd::FileDialog::Instance().IsDone("open_assets"))
                {
                    if (ifd::FileDialog::Instance().HasResult())
                    {
                        std::cout << "Opened file: '" << ifd::FileDialog::Instance().GetResult() << "'\n";
                    }
                    else
                    {
                        std::cout << "No file opened\n";
                    }

                    ifd::FileDialog::Instance().Close();
                }

                // TODO: on import, if there are multiple nodes in the asset, show a dialog to choose what to import
                // TODO: on import, if errors occur (like can not find texture file), show dialog with options to fix them (errors)

                ImGui::Begin("Shaders");

                // TODO: add shaders configuration window with the default shader

                ImGui::End();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Scene"))
            {
                ImGui::Text("Place assets in a scene space");

                ImGui::Begin("Scene structure");

                static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                static bool align_label_with_current_x_position = false;
                static bool test_drag_and_drop = false;
                /*ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow", &base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
        ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", &base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
        ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth", &base_flags, ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::SameLine();

        ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth", &base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
        ImGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
        ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);
        ImGui::Text("Hello!");
        if (align_label_with_current_x_position)
            ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());*/

                // 'selection_mask' is dumb representation of what may be user-side selection state.
                //  You may retain selection state inside or outside your objects in whatever format you see fit.
                // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
                /// of the loop. May be a pointer to your own node type, etc.
                static int selection_mask = (1 << 2);
                int node_clicked = -1;
                static int node_selected = -1;
                for (int i = 0; i < 6; i++)
                {
                    // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                    // To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
                    ImGuiTreeNodeFlags node_flags = base_flags;
                    const bool is_selected = (selection_mask & (1 << i)) != 0;
                    if (is_selected)
                        node_flags |= ImGuiTreeNodeFlags_Selected;
                    if (i < 3)
                    {
                        // Items 0..2 are Tree Node
                        bool node_open = ImGui::TreeNodeEx((void*) (intptr_t) i, node_flags, "Selectable Node %d", i);
                        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                        {
                            node_clicked = i;
                            node_selected = (node_selected == i ? -1 : i);
                        }
                        if (test_drag_and_drop && ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                            ImGui::Text("This is a drag and drop source");
                            ImGui::EndDragDropSource();
                        }
                        if (node_open)
                        {
                            ImGui::BulletText("Blah blah\nBlah Blah");
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        // Items 3..5 are Tree Leaves
                        // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                        // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;// ImGuiTreeNodeFlags_Bullet
                        ImGui::TreeNodeEx((void*) (intptr_t) i, node_flags, "Selectable Leaf %d", i);
                        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                        {
                            node_clicked = i;
                            node_selected = (node_selected == i ? -1 : i);
                        }
                        if (test_drag_and_drop && ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                            ImGui::Text("This is a drag and drop source");
                            ImGui::EndDragDropSource();
                        }
                    }
                }

                if (node_clicked != -1)
                {
                    // Update selection state
                    // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                    if (ImGui::GetIO().KeyCtrl)
                        selection_mask ^= (1 << node_clicked);// CTRL+click to toggle
                    else                                      //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                        selection_mask = (1 << node_clicked); // Click to single-select
                }

                if (align_label_with_current_x_position)
                    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

                if (node_selected != -1)
                {
                    ImGui::SeparatorText(std::format("Properties (node {})", node_selected).c_str());

                    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

                    if (ImGui::BeginTable("table1", 2, flags))
                    {
                        // Display headers so we can inspect their interaction with borders.
                        // (Headers are not the main purpose of this section of the demo, so we are not elaborating on them too much. See other sections for details)
                        if (true)
                        {
                            ImGui::TableSetupColumn("Property");
                            ImGui::TableSetupColumn("Value");
                            ImGui::TableHeadersRow();
                        }

                        for (int row = 0; row < 5; row++)
                        {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            char buf[32];
                            sprintf(buf, "Hello %d,%d", 0, row);
                            ImGui::TextUnformatted(buf);

                            ImGui::TableSetColumnIndex(1);
                            float vec3f[3];
                            if (row % 2 == 0)
                                ImGui::InputFloat3("##label", vec3f);
                            else
                                ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                        }
                        ImGui::EndTable();
                    }

                }

                ImGui::End();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Logic"))
            {
                ImGui::Text("Write logic code here");

                static TextEditor editor;
                static bool text_editor_initialized = false;

                if (!text_editor_initialized)
                {
                    ImVec4 wndBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

                    // light theme - load default icons
                    if ((wndBg.x + wndBg.y + wndBg.z) / 3.0f > 0.5f)
                    {
                        editor.SetPalette(TextEditor::GetLightPalette());
                    }
                    else
                    {
                        editor.SetPalette(TextEditor::GetDarkPalette());
                    }

                    auto lang = TextEditor::LanguageDefinition::GLSL();

                    /*
                // set your own known preprocessor symbols...
    static const char* ppnames[] = { "NULL", "PM_REMOVE",
        "ZeroMemory", "DXGI_SWAP_EFFECT_DISCARD", "D3D_FEATURE_LEVEL", "D3D_DRIVER_TYPE_HARDWARE", "WINAPI","D3D11_SDK_VERSION", "assert" };
    // ... and their corresponding values
    static const char* ppvalues[] = {
        "#define NULL ((void*)0)",
        "#define PM_REMOVE (0x0001)",
        "Microsoft's own memory zapper function\n(which is a macro actually)\nvoid ZeroMemory(\n\t[in] PVOID  Destination,\n\t[in] SIZE_T Length\n); ",
        "enum DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD = 0",
        "enum D3D_FEATURE_LEVEL",
        "enum D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE  = ( D3D_DRIVER_TYPE_UNKNOWN + 1 )",
        "#define WINAPI __stdcall",
        "#define D3D11_SDK_VERSION (7)",
        " #define assert(expression) (void)(                                                  \n"
        "    (!!(expression)) ||                                                              \n"
        "    (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \n"
        " )"
        };

    for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
    {
        TextEditor::Identifier id;
        id.mDeclaration = ppvalues[i];
        lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
    }
    */

                    // set your own identifiers
                    static const char* identifiers[] = {
                            "gl_FragDepth"};

                    static const char* idecls[] =
                            {
                                    "float gl_FragDepth"};

                    for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
                    {
                        TextEditor::Identifier id;
                        id.mDeclaration = std::string(idecls[i]);
                        lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
                    }

                    editor.SetLanguageDefinition(lang);

                    // error markers
                    TextEditor::ErrorMarkers markers;
                    markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
                    markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
                    editor.SetErrorMarkers(markers);

                    editor.SetText("#version 430\n\n\
in vec4 fragmentPosition;\n\n\
struct PointLight\n\
{\n\
                        vec3 lightPosition;\n\
                        float farPlane;\n\
                        mat4 projectionViewMatrices[6];\n\
};\n\
\n\
layout (std430, binding = 5) buffer pointLightData\n\
{\n\
                        PointLight pointLight;\n\
};\n\
\n\
void main()\n\
{\n\
                        float distance = length(fragmentPosition.xyz - pointLight.lightPosition) / pointLight.farPlane;\n\
\n\
                        gl_FragDepth = distance;\n\
}");

                    text_editor_initialized = true;
                }

                editor.Render("TextEditor");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("States"))

            {
                ImGui::Text("Define states and transitions for a scene");

                ImNodes::BeginNodeEditor();

                for (auto i = 0; i < 5; ++i)
                {
                    auto node_id = i + 1;

                    ImNodes::BeginNode(node_id);

                    ImNodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(std::format("state", node_id).c_str());
                    ImNodes::EndNodeTitleBar();

                    ImNodes::BeginInputAttribute(node_id << 8);
                    ImGui::TextUnformatted("Trigger");
                    ImNodes::EndInputAttribute();

                    ImNodes::BeginOutputAttribute(node_id << 16);
                    ImGui::PushItemWidth(120.0f);
                    char* signal_name = new char[255];
                    ImGui::InputText("Signal", signal_name, 255);
                    ImGui::PopItemWidth();
                    ImNodes::EndOutputAttribute();

                    ImNodes::BeginStaticAttribute(node_id << 24);
                    ImGui::PushItemWidth(120.0f);
                    ImGui::Button("Add signal");
                    ImGui::PopItemWidth();
                    ImNodes::EndStaticAttribute();

                    ImNodes::EndNode();
                }

                static std::vector<NodeLink> state_transitions;

                if (state_transitions.empty())
                {
                    state_transitions.push_back(NodeLink {.id = 1, .start_attr = 1 << 16, .end_attr = 3 << 8});
                    state_transitions.push_back(NodeLink {.id = 2, .start_attr = 2 << 16, .end_attr = 3 << 8});
                    state_transitions.push_back(NodeLink {.id = 3, .start_attr = 3 << 16, .end_attr = 4 << 8});
                    state_transitions.push_back(NodeLink {.id = 4, .start_attr = 3 << 16, .end_attr = 5 << 8});
                }

                for (const auto& state_transition : state_transitions)
                {
                    ImNodes::Link(state_transition.id, state_transition.start_attr, state_transition.end_attr);
                }

                ImNodes::EndNodeEditor();

                static int current_id = 0;

                {
                    static NodeLink transition;
                    if (ImNodes::IsLinkCreated(&transition.start_attr, &transition.end_attr))
                    {
                        transition.id = ++current_id;
                        state_transitions.push_back(transition);
                    }
                }

                {
                    int link_id;

                    if (ImNodes::IsLinkDestroyed(&link_id))
                    {
                        auto iter = std::find_if(
                                state_transitions.begin(), state_transitions.end(), [link_id](const NodeLink& transition) -> bool {
                                    return transition.id == link_id;
                                });

                        if (iter != state_transitions.end())
                        {
                            state_transitions.erase(iter);
                        }
                    }
                }

                {
                    static int prev_start_attribute = -1;
                    int start_attribute = -1;

                    if (prev_start_attribute != -1)
                    {
                        ImGui::OpenPopup("add state");
                    }

                    if (ImNodes::IsLinkDropped(&start_attribute) && prev_start_attribute == -1)
                    {
                        prev_start_attribute = start_attribute;
                    }

                    if (ImGui::BeginPopup("add state"))
                    {
                        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

                        if (ImGui::MenuItem("state"))
                        {
                            prev_start_attribute = -1;
                            // const Node value(NodeType::value, 0.f);
                            // const Node op(NodeType::add);

                            /*UiNode ui_node;
                                ui_node.type = UiNodeType::add;
                                ui_node.ui.add.lhs = graph_.insert_node(value);
                                ui_node.ui.add.rhs = graph_.insert_node(value);
                                ui_node.id = graph_.insert_node(op);

                                graph_.insert_edge(ui_node.id, ui_node.ui.add.lhs);
                                graph_.insert_edge(ui_node.id, ui_node.ui.add.rhs);

                                nodes_.push_back(ui_node);
                                ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);*/
                        }

                        ImGui::EndPopup();
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Rendering pipeline"))
            {
                ImGui::Text("Define scene-wise rendering pipeline (forward or deferred rendering, batch multi-draw optimizations, shadow mapping, etc.)");

                ImNodes::BeginNodeEditor();

                for (auto i = 0; i < 5; ++i)
                {
                    auto node_id = i + 1;

                    ImNodes::BeginNode(node_id);

                    ImNodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(std::format("node {}", node_id).c_str());
                    ImNodes::EndNodeTitleBar();

                    ImNodes::BeginInputAttribute(node_id << 8);
                    ImGui::TextUnformatted("input");
                    ImNodes::EndInputAttribute();

                    ImNodes::BeginStaticAttribute(node_id << 16);
                    ImGui::PushItemWidth(120.0f);
                    static float c1 = 1;
                    ImGui::DragFloat("value", &c1, 0.01f);
                    ImGui::PopItemWidth();
                    ImNodes::EndStaticAttribute();

                    ImNodes::BeginOutputAttribute(node_id << 24);
                    /*const float text_width1 = ImGui::CalcTextSize("output").x;
                    ImGui::Indent(120.f + ImGui::CalcTextSize("value").x - text_width1);
                    ImGui::TextUnformatted("output");*/
                    ImGui::PushItemWidth(120.0f);
                    static float c2 = -3.14f * i;
                    ImGui::DragFloat("value2", &c2, 0.01f);
                    ImGui::PopItemWidth();
                    ImNodes::EndOutputAttribute();

                    ImNodes::EndNode();
                }

                static std::vector<NodeLink> links;

                /*
                * Create the following diagram (temporary):
                *
                * node 1 \        / node 4
                *          node 3
                * node 2 /        \ node 5
                */

                if (links.empty())
                {
                    links.push_back(NodeLink {.id = 1, .start_attr = 1 << 24, .end_attr = 3 << 8});
                    links.push_back(NodeLink {.id = 2, .start_attr = 2 << 24, .end_attr = 3 << 8});
                    links.push_back(NodeLink {.id = 3, .start_attr = 3 << 24, .end_attr = 4 << 8});
                    links.push_back(NodeLink {.id = 4, .start_attr = 3 << 24, .end_attr = 5 << 8});
                }

                for (const auto& link : links)
                {
                    ImNodes::Link(link.id, link.start_attr, link.end_attr);
                }

                ImNodes::EndNodeEditor();

                static int current_id = 0;

                {
                    static NodeLink link;
                    if (ImNodes::IsLinkCreated(&link.start_attr, &link.end_attr))
                    {
                        link.id = ++current_id;
                        links.push_back(link);
                    }
                }

                {
                    int link_id;
                    if (ImNodes::IsLinkDestroyed(&link_id))
                    {
                        auto iter = std::find_if(
                                links.begin(), links.end(), [link_id](const NodeLink& link) -> bool {
                                    return link.id == link_id;
                                });
                        // assert(iter != editor.links.end());
                        links.erase(iter);
                    }
                }

                //ax::NodeEditor::SetCurrentEditor(m_Context);
                //ax::NodeEditor::Begin("My Editor", ImVec2(0.0, 0.0f));
                //int uniqueId = 1;
                //// Start drawing nodes.
                //ax::NodeEditor::BeginNode(uniqueId++);
                //ImGui::Text("Node A");
                //ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
                //ImGui::Text("-> In");
                //ax::NodeEditor::EndPin();
                //ImGui::SameLine();
                //ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
                //ImGui::Text("Out ->");
                //ax::NodeEditor::EndPin();
                //ax::NodeEditor::EndNode();
                //ax::NodeEditor::End();
                //ax::NodeEditor::SetCurrentEditor(nullptr);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        // glClearColor(0 * 0, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    // ax::NodeEditor::DestroyEditor(m_Context);

    ImNodes::DestroyContext();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
