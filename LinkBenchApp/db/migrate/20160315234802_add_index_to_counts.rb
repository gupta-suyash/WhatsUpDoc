class AddIndexToCounts < ActiveRecord::Migration
  def change
	add_index :counts, [:id, :link_type], unique: true
  end
end
