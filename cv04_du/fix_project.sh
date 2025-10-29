# a) Scene.h — defaultně prázdné, aby vše ostatní nemusel řešit
awk '{
  print
} /class Scene/ && !seen {
  seen=1
}
/virtual void render/ && !added {
  print "    // doplněno kvůli Phong/Blinn: kamera->FS uViewPos"
  print "    virtual void setViewPos(const glm::vec3&) {}"
  added=1
}' src/scene/Scene.h > tmp && mv tmp src/scene/Scene.h

# b) SpheresScene.h — přidej override
grep -q "setViewPos" src/scene/SpheresScene.h || \
awk '{
  print
} /class SpheresScene/ && !done {
  print "    void setViewPos(const glm::vec3&) override;"
  done=1
}' src/scene/SpheresScene.h > tmp && mv tmp src/scene/SpheresScene.h

# c) SpheresScene.cpp — implementace setteru a použití proměnné viewPos_
awk '{
  if($0 ~ /^SpheresScene::SpheresScene\(\)/){ seen=1 }
  print
} END {
}' src/scene/SpheresScene.cpp > tmp && mv tmp src/scene/SpheresScene.cpp

# vlož implementaci setteru (pokud tam ještě není)
grep -q "SpheresScene::setViewPos" src/scene/SpheresScene.cpp || \
cat >> src/scene/SpheresScene.cpp <<'EOF'

// --- doplněno: viewPos z kamery (pro Phong/Blinn)
void SpheresScene::setViewPos(const glm::vec3& p){
    viewPos_ = p;
}
EOF
